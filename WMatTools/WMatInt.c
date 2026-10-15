#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "WMatInt.h"

WMatInt* create_WMatInt(int w, int h) {
	assert(w > 0 && h > 0);

	WMatInt* mat = malloc(sizeof(WMatInt));
	assert(mat != NULL);

	mat->width = w;
	mat->height = h;
	mat->data = malloc((size_t)w * (size_t)h * sizeof(int));
	assert(mat->data != NULL);

	return mat;
}

static int isBlankLine(const char* line) {
	while (*line == ' ' || *line == '\t') {
		line++;
	}
	return *line == '\0';
}

// Parses one comma separated line of integers. Writes the values to out
// when it is not NULL. Returns the number of values on the line.
static int parseCsvLine(const char* line, int* out) {
	int count = 0;
	const char* p = line;
	for (;;) {
		char* end;
		long value = strtol(p, &end, 10);
		assert(end != p && "expected an integer");
		assert(value >= INT_MIN && value <= INT_MAX);
		if (out != NULL) {
			out[count] = (int)value;
		}
		count++;

		p = end;
		while (*p == ' ' || *p == '\t') {
			p++;
		}
		if (*p == '\0') {
			return count;
		}
		assert(*p == ',' && "expected a comma between values");
		p++;
	}
}

WMatInt* setData_WMatInt(const char* path) {
	assert(path != NULL);

	// Read the whole file into memory
	FILE* file = fopen(path, "rb");
	assert(file != NULL && "could not open file");
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	assert(size >= 0);
	fseek(file, 0, SEEK_SET);

	char* text = malloc((size_t)size + 1);
	assert(text != NULL);
	size_t bytesRead = fread(text, 1, (size_t)size, file);
	fclose(file);
	assert(bytesRead == (size_t)size);
	text[bytesRead] = '\0';

	// Split into lines in place. Handles both \n and \r\n endings; the
	// empty strings left by \r\n are skipped as blank lines.
	char* textEnd = text + bytesRead;
	for (char* c = text; c < textEnd; c++) {
		if (*c == '\n' || *c == '\r') {
			*c = '\0';
		}
	}

	// First pass: find the size and check every row has the same width
	int width = 0;
	int height = 0;
	for (char* line = text; line < textEnd; line += strlen(line) + 1) {
		if (isBlankLine(line)) {
			continue;
		}
		int count = parseCsvLine(line, NULL);
		if (height == 0) {
			width = count;
		}
		assert(count == width && "every row must have the same number of values");
		height++;
	}
	assert(height > 0 && "file contains no values");

	// Second pass: fill the matrix
	WMatInt* mat = create_WMatInt(width, height);
	int row = 0;
	for (char* line = text; line < textEnd; line += strlen(line) + 1) {
		if (isBlankLine(line)) {
			continue;
		}
		parseCsvLine(line, getAtRow_WMatInt(mat, row));
		row++;
	}

	free(text);
	return mat;
}

WMatInt* copy_WMatInt(WMatInt* input) {
	assert(input != NULL);

	WMatInt* mat = create_WMatInt(input->width, input->height);
	memcpy(mat->data, input->data,
	       (size_t)input->width * (size_t)input->height * sizeof(int));
	return mat;
}

WMatInt* submat_WMatInt(WMatInt* input, int x, int y, int w, int h) {
	assert(input != NULL);
	assert(x >= 0 && y >= 0 && w > 0 && h > 0);
	assert(x + w <= input->width && y + h <= input->height);

	WMatInt* mat = create_WMatInt(w, h);
	for (int row = 0; row < h; row++) {
		memcpy(getAtRow_WMatInt(mat, row),
		       getAtRow_WMatInt(input, y + row) + x,
		       (size_t)w * sizeof(int));
	}
	return mat;
}

WMatInt* concat_WMatInt(WMatInt* a, WMatInt* b, int axis) {
	assert(a != NULL && b != NULL);
	assert(axis == 0 || axis == 1);

	WMatInt* mat;
	if (axis == 0) {
		// b below a: the two data arrays are simply placed back to back
		assert(a->width == b->width);
		mat = create_WMatInt(a->width, a->height + b->height);
		size_t aCount = (size_t)a->width * (size_t)a->height;
		size_t bCount = (size_t)b->width * (size_t)b->height;
		memcpy(mat->data, a->data, aCount * sizeof(int));
		memcpy(mat->data + aCount, b->data, bCount * sizeof(int));
	} else {
		// b right of a: each output row is a row of a followed by a row of b
		assert(a->height == b->height);
		mat = create_WMatInt(a->width + b->width, a->height);
		for (int row = 0; row < a->height; row++) {
			int* outRow = getAtRow_WMatInt(mat, row);
			memcpy(outRow, getAtRow_WMatInt(a, row),
			       (size_t)a->width * sizeof(int));
			memcpy(outRow + a->width, getAtRow_WMatInt(b, row),
			       (size_t)b->width * sizeof(int));
		}
	}
	return mat;
}

// Work given to one thread: computes h rows of C = A * B.
// All three blocks are row-major. Adata rows are n ints long;
// Bdata and Cdata rows are w ints long.
typedef struct WFastMatInt {
	int h; // Height of sub-mat of A
	int n; // Width and height of A and B
	int w; // Width of B and C
	const int* Adata;
	const int* Bdata;
	int* Cdata;
} WFastMatInt;

static void* fastMatMultWorker(void* arg)
{
	assert(arg != NULL);
	WFastMatInt* data = (WFastMatInt*)arg;
	const int h = data->h;
	const int n = data->n;
	const int w = data->w;
	const int* Adata = data->Adata;
	const int* Bdata = data->Bdata;
	int* Cdata = data->Cdata;
	
	assert(Adata != NULL && Bdata != NULL && Cdata != NULL);
	
	// i-k-j loop order walks every array along its rows, which is
	// cache friendly for row-major storage.
	for (int i = 0; i < h; i++) {
		//Init CData to zero
		for (int j = 0; j < w; j++) {
			Cdata[i*w+j] = 0;
		}
		for (int k = 0; k < n; k++) {
			const int aVal = Adata[i*n + k];
			for (int j = 0; j < w; j++) {
				Cdata[i*w+j] += aVal * Bdata[k*w + j];
			}
		}
	}
	
	return NULL;
}

WMatInt* fastMatMult_WMatInt(WMatInt* a, WMatInt* b) 
{
	assert(a->width == b->height);
	const int numThreads = 4;
	if(a->height < 10 || a->height < numThreads) {
		return matMult_WMatInt(a , b);
	}
	
	WFastMatInt* workerData = malloc(sizeof(WFastMatInt) * numThreads);
	const int minChop = (int)(a->height / numThreads);
	
	pthread_t* threadIDs = malloc(sizeof(pthread_t) * numThreads);
	
	int* Cdata = malloc(sizeof(int)*a->height*b->width);
	for(int i = 0; i < numThreads; i++){
		workerData[i].h = i == 0 ? minChop + a->height % numThreads : minChop;
		workerData[i].n = a->width;
		workerData[i].w = b->width;
		
		const int offset = (i == 0 ? 0 : (workerData[0].h + (i-1)*minChop)); 
		workerData[i].Adata = getAtRow_WMatInt(a, offset);
		workerData[i].Bdata = b->data;
		workerData[i].Cdata = Cdata+offset*b->width;
		
		//printf("WorkerData %i: h(%i), n(%i), w(%i), Adata(%i), Bdata(%i), Cdata(%i)\n", i, workerData[i].h, workerData[i].n, workerData[i].w, offset, 0, offset);
		//fastMatMultWorker(&workerData[i]);
		
		if(pthread_create(&threadIDs[i], NULL, fastMatMultWorker, &workerData[i]) != 0)
		{
			threadIDs[i] = -1;
		}
	}
	
	for(int i = 0; i < numThreads; i++){
		//we could have some kind of error on failure
		if(threadIDs[i] > 0) {
			//This returns zero on success
			pthread_join(threadIDs[i], NULL);
		}
	}
	
	WMatInt* res = malloc(sizeof(WMatInt));
	res->height = a->height;
	res->width =  b->width;
	res->data = Cdata;
	free(workerData);
	free(threadIDs); 
	
	return res;
}

void delete_WMatInt(WMatInt* doneMat) {
	if (doneMat == NULL) {
		return;
	}
	free(doneMat->data);
	free(doneMat);
}

int* getAtRow_WMatInt(WMatInt* mat, int rowNum) {
	assert(mat != NULL);
	assert(rowNum >= 0 && rowNum < mat->height);

	return &mat->data[rowNum * mat->width];
}

int getData_WMatInt(WMatInt* mat, int x, int y) {
	assert(mat != NULL);
	assert(x >= 0 && x < mat->width);
	assert(y >= 0 && y < mat->height);

	return mat->data[y * mat->width + x];
}

WMatInt* makeZeros_WMatInt(int w, int h) {
	WMatInt* mat = create_WMatInt(w, h);
	memset(mat->data, 0, (size_t)w * (size_t)h * sizeof(int));
	return mat;
}

WMatInt* makeOnes_WMatInt(int w, int h) {
	WMatInt* mat = create_WMatInt(w, h);
	int count = w * h;
	for (int i = 0; i < count; i++) {
		mat->data[i] = 1;
	}
	return mat;
}

WMatInt* makeEye_WMatInt(int size) {
	WMatInt* mat = makeZeros_WMatInt(size, size);
	for (int i = 0; i < size; i++) {
		mat->data[i * size + i] = 1;
	}
	return mat;
}

WMatInt* matMult_WMatInt(WMatInt* a, WMatInt* b) {
	assert(a != NULL && b != NULL);
	assert(a->width == b->height);

	WMatInt* result = makeZeros_WMatInt(b->width, a->height);

	// i-k-j loop order walks every array along its rows, which is
	// cache friendly for row-major storage.
	for (int i = 0; i < a->height; i++) {
		int* resultRow = getAtRow_WMatInt(result, i);
		int* aRow = getAtRow_WMatInt(a, i);
		for (int k = 0; k < a->width; k++) {
			int aVal = aRow[k];
			int* bRow = getAtRow_WMatInt(b, k);
			for (int j = 0; j < b->width; j++) {
				resultRow[j] += aVal * bRow[j];
			}
		}
	}

	return result;
}

int matDot_WMatInt(WMatInt* a, WMatInt* b) {
	assert(a != NULL && b != NULL);
	assert(a->width == b->width && a->height == b->height);

	int sum = 0;
	int count = a->width * a->height;
	for (int i = 0; i < count; i++) {
		sum += a->data[i] * b->data[i];
	}
	return sum;
}

WMatInt* constScale_WMatInt(int scale, WMatInt* b) {
	WMatInt* mat = copy_WMatInt(b);
	inplaceConstScale_WMatInt(scale, mat);
	return mat;
}

void inplaceConstScale_WMatInt(int scale, WMatInt* b) {
	assert(b != NULL);

	int count = b->width * b->height;
	for (int i = 0; i < count; i++) {
		b->data[i] *= scale;
	}
}

// Where the element at flat index i ends up after transposing a matrix
// with the given height and count elements. Element (row r, col c) sits
// at i = r*w + c and moves to c*h + r, which works out to i*h mod (count-1).
// The first and last elements never move.
static int transposeDest(int i, int height, int count) {
	if (i == count - 1) {
		return i;
	}
	return (int)(((long long)i * height) % (count - 1));
}

void inplaceTranspose_WMatInt(WMatInt* input) {
	assert(input != NULL);

	int count = input->width * input->height;

	// Every element moves along a cycle of positions. Rotate each cycle
	// once, starting from its smallest index (its "leader"). Finding the
	// leader by walking the cycle avoids needing a visited array.
	for (int start = 1; start < count - 1; start++) {
		int next = transposeDest(start, input->height, count);
		while (next > start) {
			next = transposeDest(next, input->height, count);
		}
		if (next != start) {
			continue; // cycle already rotated from a smaller index
		}

		int carried = input->data[start];
		int cur = start;
		do {
			int dest = transposeDest(cur, input->height, count);
			int displaced = input->data[dest];
			input->data[dest] = carried;
			carried = displaced;
			cur = dest;
		} while (cur != start);
	}

	int oldWidth = input->width;
	input->width = input->height;
	input->height = oldWidth;
}

WMatInt* transpose_WMatInt(WMatInt* input) {
	assert(input != NULL);

	WMatInt* mat = create_WMatInt(input->height, input->width);
	for (int y = 0; y < input->height; y++) {
		int* inRow = getAtRow_WMatInt(input, y);
		for (int x = 0; x < input->width; x++) {
			mat->data[x * mat->width + y] = inRow[x];
		}
	}
	return mat;
}

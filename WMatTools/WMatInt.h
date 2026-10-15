#ifndef WMATINT_H
#define WMATINT_H

#ifdef __cplusplus
extern "C" {
#endif

// Integer matrix. Data is stored in a single row-major array:
// element (row y, column x) lives at data[y * width + x].
typedef struct WMatInt {
	int width;  // number of columns
	int height; // number of rows
	int* data;
} WMatInt;

// Allocates a w x h matrix. Contents are uninitialized.
WMatInt* create_WMatInt(int w, int h);

// Returns a new matrix loaded from a CSV text file: one matrix row per
// line, integer values separated by commas. Every row must have the
// same number of values. Blank lines are ignored.
WMatInt* setData_WMatInt(const char* path);

// Returns a new matrix with the same size and contents as input.
WMatInt* copy_WMatInt(WMatInt* input);
// Returns a new w x h matrix copied from input, with its top-left
// corner at column x, row y. The region must lie inside input.
WMatInt* submat_WMatInt(WMatInt* input, int x, int y, int w, int h);
// Returns a new matrix joining a and b.
// axis 0: b is stacked below a (widths must match).
// axis 1: b is placed to the right of a (heights must match).
WMatInt* concat_WMatInt(WMatInt* a, WMatInt* b, int axis);

void delete_WMatInt(WMatInt* doneMat);

// Returns a pointer to the start of a row, so mat elements can be
// accessed like a 2D array: getAtRow_WMatInt(mat, y)[x]
int* getAtRow_WMatInt(WMatInt* mat, int rowNum);
// Returns the element at column x, row y.
int getData_WMatInt(WMatInt* mat, int x, int y);

WMatInt* makeZeros_WMatInt(int w, int h);
WMatInt* makeOnes_WMatInt(int w, int h);
WMatInt* makeEye_WMatInt(int size);

// Matrix product a * b. Requires a->width == b->height.
WMatInt* matMult_WMatInt(WMatInt* a, WMatInt* b);

// FasterMatrix product a * b. Requires a->width == b->height.
WMatInt* fastMatMult_WMatInt(WMatInt* a, WMatInt* b);

// Sum of element-wise products. Requires a and b to be the same size.
int matDot_WMatInt(WMatInt* a, WMatInt* b);

// Returns a new matrix with every element of b multiplied by scale.
WMatInt* constScale_WMatInt(int scale, WMatInt* b);
// Multiplies every element of b by scale.
void inplaceConstScale_WMatInt(int scale, WMatInt* b);

// Transposes input in place without allocating, for any shape.
// Width and height are swapped.
void inplaceTranspose_WMatInt(WMatInt* input);
// Returns a new matrix that is the transpose of input.
WMatInt* transpose_WMatInt(WMatInt* input);

#ifdef __cplusplus
}
#endif

#endif

#include <gtest/gtest.h>

#include <cstdio>

#include "WMatInt.h"
#include "WMatIntTools.h"

// Writes text to a file in binary mode so line endings are exactly as given.
static void writeFile(const char* path, const char* text) {
	FILE* file = fopen(path, "wb");
	ASSERT_NE(file, nullptr);
	fputs(text, file);
	fclose(file);
}

// Fills a matrix from a row-major list of values.
static void fill(WMatInt* mat, const int* values) {
	for (int y = 0; y < mat->height; y++) {
		int* row = getAtRow_WMatInt(mat, y);
		for (int x = 0; x < mat->width; x++) {
			row[x] = values[y * mat->width + x];
		}
	}
}

TEST(MatTest, createAndFillMat) {
	WMatInt* mat = create_WMatInt(3, 2);
	ASSERT_NE(mat, nullptr);
	EXPECT_EQ(mat->width, 3);
	EXPECT_EQ(mat->height, 2);

	const int values[] = {1, 2, 3,
	                      4, 5, 6};
	fill(mat, values);

	// 2D-style access matches the single backing array
	EXPECT_EQ(getData_WMatInt(mat, 0, 0), 1);
	EXPECT_EQ(getData_WMatInt(mat, 2, 0), 3);
	EXPECT_EQ(getData_WMatInt(mat, 0, 1), 4);
	EXPECT_EQ(getData_WMatInt(mat, 2, 1), 6);
	EXPECT_EQ(getAtRow_WMatInt(mat, 1)[1], 5);
	EXPECT_EQ(mat->data[1 * 3 + 2], 6);

	delete_WMatInt(mat);
}

TEST(MatTest, makeZerosOnesEye) {
	WMatInt* zeros = makeZeros_WMatInt(4, 3);
	WMatInt* ones = makeOnes_WMatInt(4, 3);
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 4; x++) {
			EXPECT_EQ(getData_WMatInt(zeros, x, y), 0);
			EXPECT_EQ(getData_WMatInt(ones, x, y), 1);
		}
	}

	WMatInt* eye = makeEye_WMatInt(3);
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			EXPECT_EQ(getData_WMatInt(eye, x, y), x == y ? 1 : 0);
		}
	}

	delete_WMatInt(zeros);
	delete_WMatInt(ones);
	delete_WMatInt(eye);
}

TEST(MatTest, matMultiply) {
	// a is 2x3 (2 rows, 3 cols), b is 3x2 (3 rows, 2 cols)
	WMatInt* a = create_WMatInt(3, 2);
	WMatInt* b = create_WMatInt(2, 3);
	const int aVals[] = {1, 2, 3,
	                     4, 5, 6};
	const int bVals[] = { 7,  8,
	                      9, 10,
	                     11, 12};
	fill(a, aVals);
	fill(b, bVals);

	WMatInt* c = matMult_WMatInt(a, b);
	ASSERT_EQ(c->width, 2);
	ASSERT_EQ(c->height, 2);
	EXPECT_EQ(getData_WMatInt(c, 0, 0), 58);
	EXPECT_EQ(getData_WMatInt(c, 1, 0), 64);
	EXPECT_EQ(getData_WMatInt(c, 0, 1), 139);
	EXPECT_EQ(getData_WMatInt(c, 1, 1), 154);

	// Multiplying by identity leaves the matrix unchanged
	WMatInt* eye = makeEye_WMatInt(3);
	WMatInt* same = matMult_WMatInt(a, eye);
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(same->data[i], aVals[i]);
	}

	delete_WMatInt(a);
	delete_WMatInt(b);
	delete_WMatInt(c);
	delete_WMatInt(eye);
	delete_WMatInt(same);
}

TEST(MatTest, dotProduct) {
	WMatInt* a = create_WMatInt(3, 1);
	WMatInt* b = create_WMatInt(3, 1);
	const int aVals[] = {1, 2, 3};
	const int bVals[] = {4, 5, 6};
	fill(a, aVals);
	fill(b, bVals);

	EXPECT_EQ(matDot_WMatInt(a, b), 32);

	WMatInt* ones = makeOnes_WMatInt(2, 2);
	WMatInt* eye = makeEye_WMatInt(2);
	EXPECT_EQ(matDot_WMatInt(ones, eye), 2);

	delete_WMatInt(a);
	delete_WMatInt(b);
	delete_WMatInt(ones);
	delete_WMatInt(eye);
}

// Builds a w x h matrix holding 0, 1, 2, ... in row-major order.
static WMatInt* makeCounting(int w, int h) {
	WMatInt* mat = create_WMatInt(w, h);
	for (int i = 0; i < w * h; i++) {
		mat->data[i] = i;
	}
	return mat;
}

TEST(MatTest, copy) {
	WMatInt* src = makeCounting(3, 2);
	WMatInt* dup = copy_WMatInt(src);
	ASSERT_EQ(dup->width, 3);
	ASSERT_EQ(dup->height, 2);
	EXPECT_NE(dup->data, src->data);
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(dup->data[i], i);
	}

	// Copy is independent of the source
	dup->data[0] = 99;
	EXPECT_EQ(src->data[0], 0);

	delete_WMatInt(src);
	delete_WMatInt(dup);
}

TEST(MatTest, submat) {
	// 0  1  2  3
	// 4  5  6  7
	// 8  9 10 11
	WMatInt* src = makeCounting(4, 3);
	WMatInt* sub = submat_WMatInt(src, 1, 1, 2, 2);
	ASSERT_EQ(sub->width, 2);
	ASSERT_EQ(sub->height, 2);
	EXPECT_EQ(getData_WMatInt(sub, 0, 0), 5);
	EXPECT_EQ(getData_WMatInt(sub, 1, 0), 6);
	EXPECT_EQ(getData_WMatInt(sub, 0, 1), 9);
	EXPECT_EQ(getData_WMatInt(sub, 1, 1), 10);

	// Whole matrix is a valid region
	WMatInt* whole = submat_WMatInt(src, 0, 0, 4, 3);
	for (int i = 0; i < 12; i++) {
		EXPECT_EQ(whole->data[i], i);
	}

	delete_WMatInt(src);
	delete_WMatInt(sub);
	delete_WMatInt(whole);
}

TEST(MatTest, concat) {
	WMatInt* a = makeCounting(2, 2);           // 0 1 / 2 3
	WMatInt* b = makeOnes_WMatInt(2, 1);       // 1 1
	WMatInt* c = makeZeros_WMatInt(3, 2);      // 0 0 0 / 0 0 0

	WMatInt* vert = concat_WMatInt(a, b, 0);
	ASSERT_EQ(vert->width, 2);
	ASSERT_EQ(vert->height, 3);
	const int vertExpected[] = {0, 1,
	                            2, 3,
	                            1, 1};
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(vert->data[i], vertExpected[i]);
	}

	WMatInt* horiz = concat_WMatInt(a, c, 1);
	ASSERT_EQ(horiz->width, 5);
	ASSERT_EQ(horiz->height, 2);
	const int horizExpected[] = {0, 1, 0, 0, 0,
	                             2, 3, 0, 0, 0};
	for (int i = 0; i < 10; i++) {
		EXPECT_EQ(horiz->data[i], horizExpected[i]);
	}

	delete_WMatInt(a);
	delete_WMatInt(b);
	delete_WMatInt(c);
	delete_WMatInt(vert);
	delete_WMatInt(horiz);
}

TEST(MatTest, constScale) {
	WMatInt* src = makeCounting(3, 2);

	WMatInt* scaled = constScale_WMatInt(-3, src);
	EXPECT_NE(scaled, src);
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(scaled->data[i], -3 * i);
		EXPECT_EQ(src->data[i], i); // source untouched
	}

	inplaceConstScale_WMatInt(2, src);
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(src->data[i], 2 * i);
	}

	delete_WMatInt(src);
	delete_WMatInt(scaled);
}

TEST(MatTest, transpose) {
	WMatInt* src = makeCounting(3, 2);   // 0 1 2 / 3 4 5
	WMatInt* t = transpose_WMatInt(src);
	ASSERT_EQ(t->width, 2);
	ASSERT_EQ(t->height, 3);
	const int expected[] = {0, 3,
	                        1, 4,
	                        2, 5};
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(t->data[i], expected[i]);
	}

	inplaceTranspose_WMatInt(src);
	ASSERT_EQ(src->width, 2);
	ASSERT_EQ(src->height, 3);
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(src->data[i], expected[i]);
	}

	delete_WMatInt(src);
	delete_WMatInt(t);
}

TEST(MatTest, inplaceTransposeAllShapes) {
	// Compare against the out-of-place version for many shapes, since the
	// in-place cycle logic is the easiest part to get subtly wrong.
	for (int w = 1; w <= 9; w++) {
		for (int h = 1; h <= 9; h++) {
			WMatInt* mat = makeCounting(w, h);
			WMatInt* expected = transpose_WMatInt(mat);
			inplaceTranspose_WMatInt(mat);
			ASSERT_EQ(mat->width, h);
			ASSERT_EQ(mat->height, w);
			for (int i = 0; i < w * h; i++) {
				ASSERT_EQ(mat->data[i], expected->data[i])
					<< "shape " << w << "x" << h << " index " << i;
			}
			delete_WMatInt(mat);
			delete_WMatInt(expected);
		}
	}
}

TEST(MatTest, setDataFromCsv) {
	const char* path = "wmat_test_basic.csv";
	writeFile(path, "1,2,3\n4,5,6\n");

	WMatInt* mat = setData_WMatInt(path);
	ASSERT_EQ(mat->width, 3);
	ASSERT_EQ(mat->height, 2);
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(mat->data[i], i + 1);
	}

	delete_WMatInt(mat);
	remove(path);
}

TEST(MatTest, setDataTolerantFormatting) {
	// CRLF endings, spaces around values, negatives, blank lines,
	// and no newline at the end of the file
	const char* path = "wmat_test_format.csv";
	writeFile(path, "\r\n 10 , -20\r\n\r\n-30,40 \r\n  \r\n7,8");

	WMatInt* mat = setData_WMatInt(path);
	ASSERT_EQ(mat->width, 2);
	ASSERT_EQ(mat->height, 3);
	const int expected[] = { 10, -20,
	                        -30,  40,
	                          7,   8};
	for (int i = 0; i < 6; i++) {
		EXPECT_EQ(mat->data[i], expected[i]);
	}

	delete_WMatInt(mat);
	remove(path);
}

TEST(MatTest, setDataSingleValue) {
	const char* path = "wmat_test_single.csv";
	writeFile(path, "42\n");

	WMatInt* mat = setData_WMatInt(path);
	ASSERT_EQ(mat->width, 1);
	ASSERT_EQ(mat->height, 1);
	EXPECT_EQ(mat->data[0], 42);

	delete_WMatInt(mat);
	remove(path);
}

TEST(MatToolsTest, bilinearSampleTest) {
	WMatInt* mat = makeCounting(3, 3);
	
	WMatInt* matScaled = constScale_WMatInt(28, mat);
	
	printf("\nBefore:\n");
	print_WMatInt(matScaled);
	WMatInt* res = bilinearSample(matScaled, 4, 4);
	printf("\nAfter:\n");
	print_WMatInt(res);
	printf("\n");
	
	delete_WMatInt(mat);
	delete_WMatInt(matScaled);
	delete_WMatInt(res);
}

#ifndef NDEBUG
TEST(MatDeathTest, setDataBadFilesAssert) {
	EXPECT_DEATH(setData_WMatInt("wmat_does_not_exist.csv"), "");

	const char* ragged = "wmat_test_ragged.csv";
	writeFile(ragged, "1,2,3\n4,5\n");
	EXPECT_DEATH(setData_WMatInt(ragged), "");
	remove(ragged);

	const char* notNumber = "wmat_test_nan.csv";
	writeFile(notNumber, "1,x,3\n");
	EXPECT_DEATH(setData_WMatInt(notNumber), "");
	remove(notNumber);

	const char* empty = "wmat_test_empty.csv";
	writeFile(empty, "\n\n");
	EXPECT_DEATH(setData_WMatInt(empty), "");
	remove(empty);
}

TEST(MatDeathTest, mismatchedSizesAssert) {
	WMatInt* a = makeOnes_WMatInt(3, 2);
	WMatInt* b = makeOnes_WMatInt(3, 2);
	EXPECT_DEATH(matMult_WMatInt(a, b), "");
	delete_WMatInt(a);
	delete_WMatInt(b);
}
#endif

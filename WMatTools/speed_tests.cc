#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>

#include <Eigen/Dense>

#include "WMatInt.h"

// Set by CMake to the absolute path of the repo's data folder
#ifndef WMAT_DATA_DIR
#define WMAT_DATA_DIR "data"
#endif

using Clock = std::chrono::steady_clock;

static double secondsSince(Clock::time_point start) {
	return std::chrono::duration<double>(Clock::now() - start).count();
}

TEST(SpeedTest, multiplyAndSum) {
	Clock::time_point total = Clock::now();

	Clock::time_point step = Clock::now();
	WMatInt* a = setData_WMatInt(WMAT_DATA_DIR "/matA.csv");
	WMatInt* b = setData_WMatInt(WMAT_DATA_DIR "/matB.csv");
	double loadTime = secondsSince(step);
	ASSERT_EQ(a->width, b->height);

	step = Clock::now();
	WMatInt* c = fastMatMult_WMatInt(a, b);
	double multTime = secondsSince(step);

	// Sum every element of c with two products of ones matrices:
	// (1 x h ones) * c gives the column sums, then * (w x 1 ones)
	// adds those up into a single 1 x 1 value.
	step = Clock::now();
	WMatInt* onesRow = makeOnes_WMatInt(c->height, 1);
	WMatInt* onesCol = makeOnes_WMatInt(1, c->width);
	WMatInt* colSums = matMult_WMatInt(onesRow, c);
	WMatInt* total1x1 = matMult_WMatInt(colSums, onesCol);
	int sum = getData_WMatInt(total1x1, 0, 0);
	double sumTime = secondsSince(step);

	printf("A: %d x %d (rows x cols), B: %d x %d, C: %d x %d\n",
	       a->height, a->width, b->height, b->width, c->height, c->width);
	printf("Sum of A x B: %d\n", sum);
	printf("Load:     %8.3f s\n", loadTime);
	printf("Multiply: %8.3f s\n", multTime);
	printf("Sum:      %8.3f s\n", sumTime);
	printf("Total:    %8.3f s\n", secondsSince(total));

	delete_WMatInt(a);
	delete_WMatInt(b);
	delete_WMatInt(c);
	delete_WMatInt(onesRow);
	delete_WMatInt(onesCol);
	delete_WMatInt(colSums);
	delete_WMatInt(total1x1);
}

// WMatInt stores its data row-major, so Eigen can view it in place.
using RowMajorMatXi =
	Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

// Same workload as multiplyAndSum, done with Eigen using at most
// maxThreads threads (0 means as many as OpenMP allows).
static void eigenMultiplyAndSum(int maxThreads) {
	Clock::time_point total = Clock::now();

	// Load with the same CSV reader so only the multiply differs
	Clock::time_point step = Clock::now();
	WMatInt* a = setData_WMatInt(WMAT_DATA_DIR "/matA.csv");
	WMatInt* b = setData_WMatInt(WMAT_DATA_DIR "/matB.csv");
	double loadTime = secondsSince(step);
	ASSERT_EQ(a->width, b->height);

	// Wrap the existing arrays without copying them
	Eigen::Map<const RowMajorMatXi> eigenA(a->data, a->height, a->width);
	Eigen::Map<const RowMajorMatXi> eigenB(b->data, b->height, b->width);

	Eigen::setNbThreads(maxThreads);

	step = Clock::now();
	Eigen::MatrixXi c(eigenA.rows(), eigenB.cols());
	c.noalias() = eigenA * eigenB;
	double multTime = secondsSince(step);

	step = Clock::now();
	int sum = c.sum();
	double sumTime = secondsSince(step);

	printf("Eigen threads: %d\n", Eigen::nbThreads());
	printf("Sum of A x B: %d\n", sum);
	printf("Load:     %8.3f s\n", loadTime);
	printf("Multiply: %8.3f s\n", multTime);
	printf("Sum:      %8.3f s\n", sumTime);
	printf("Total:    %8.3f s\n", secondsSince(total));

	delete_WMatInt(a);
	delete_WMatInt(b);
}

TEST(SpeedTest, eigenSingleThread) {
	eigenMultiplyAndSum(1);
}

TEST(SpeedTest, eigenMultiThread) {
	eigenMultiplyAndSum(0);
}

"""Create and multiply CSV matrix files used with setData_WMatInt.

Make a file with n rows and m columns, with every value drawn uniformly
from the inclusive range [a, b]:
    python make_random_csv.py n m a b output.csv [--seed SEED]

Multiply the matrices in two files and print the result:
    python make_random_csv.py multiply a.csv b.csv

Examples:
    python make_random_csv.py 3 4 -10 10 mat.csv
    python make_random_csv.py multiply a.csv b.csv
"""

import argparse
import random
import sys

import numpy as np


def load_csv_matrix(path):
    """Load a comma separated file of integers as a 2D int64 array."""
    return np.loadtxt(path, delimiter=",", dtype=np.int64, ndmin=2)


def multiplyAndSum_csv(path_a, path_b):
    """Return the matrix product of the matrices stored in two CSV files.

    Requires the number of columns in a to equal the number of rows in b.
    Uses 64-bit integers, so results are exact unless they exceed int64.
    """
    a = load_csv_matrix(path_a)
    b = load_csv_matrix(path_b)
    if a.shape[1] != b.shape[0]:
        raise ValueError(
            "cannot multiply {}x{} by {}x{} (rows x columns): "
            "columns of a must equal rows of b".format(
                a.shape[0], a.shape[1], b.shape[0], b.shape[1]))
    return np.sum(a @ b)


def make_random_csv(argv):
    parser = argparse.ArgumentParser(
        description="Write an n x m CSV of random integers in [a, b].")
    parser.add_argument("n", type=int, help="number of rows")
    parser.add_argument("m", type=int, help="number of columns")
    parser.add_argument("a", type=int, help="smallest possible value")
    parser.add_argument("b", type=int, help="largest possible value")
    parser.add_argument("output", help="path of the CSV file to write")
    parser.add_argument("--seed", type=int,
                        help="random seed, for repeatable output")
    args = parser.parse_args(argv)

    if args.n <= 0 or args.m <= 0:
        parser.error("n and m must be positive")
    if args.a > args.b:
        parser.error("a must be less than or equal to b")

    rng = random.Random(args.seed)

    with open(args.output, "w", newline="") as f:
        for _ in range(args.n):
            row = (str(rng.randint(args.a, args.b)) for _ in range(args.m))
            f.write(",".join(row) + "\n")


def print_product(argv):
    parser = argparse.ArgumentParser(
        prog="make_random_csv.py multiply",
        description="Print the matrix product of two CSV matrix files.")
    parser.add_argument("a", help="CSV file for the left matrix")
    parser.add_argument("b", help="CSV file for the right matrix")
    args = parser.parse_args(argv)

    try:
        product = multiplyAndSum_csv(args.a, args.b)
    except ValueError as e:
        parser.error(str(e))
    print(product)


def main():
    argv = sys.argv[1:]
    if argv and argv[0] == "multiply":
        print_product(argv[1:])
    else:
        make_random_csv(argv)


if __name__ == "__main__":
    main()

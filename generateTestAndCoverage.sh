#!/bin/sh

# To use this script the following packages are needed
# lcov, gcov

CURRENT_DIR=${PWD}
mkdir build
cd build
cmake ..
make
./BundleAgent_test
lcov -c -i -d . -o .coverage.base
lcov -c -d . -o .coverage.run
lcov -d . -a .coverage.base -a .coverage.run -o .coverage.total
genhtml --no-branch-coverage -o coverage_html .coverage.total
rm -f .coverage.base .coverage.run .coverage.total

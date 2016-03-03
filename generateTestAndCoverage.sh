#!/bin/sh

# To use this script the following packages are needed
# lcov, gcov

CURRENT_DIR=${PWD}
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Profile
make
./Test/BundleAgent_test
RET=$?
lcov --quiet -c -i -d . -o .coverage.base
lcov --quiet -c -d . -o .coverage.run
lcov --quiet --remove .coverage.base "*gtest*" -o .coverage.base
lcov --quiet --remove .coverage.base "/usr*" -o .coverage.base
lcov --quiet --remove .coverage.run "*gtest*" -o .coverage.run
lcov --quiet --remove .coverage.run "/usr*" -o .coverage.run
lcov --quiet -d . -a .coverage.base -a .coverage.run -o .coverage.total
genhtml --quiet --no-branch-coverage -o coverage_html .coverage.total
rm -f .coverage.base .coverage.run .coverage.total
exit $RET
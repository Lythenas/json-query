#!/usr/bin/env bash
# created using https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake

set -xe

make -B COVERAGE=1 jsonquery_test

# run test
LLVM_PROFILE_FILE="jsonquery_test.profraw" ./jsonquery_test

# generate coverage report
llvm-profdata merge -sparse jsonquery_test.profraw -o jsonquery_test.profdata
llvm-cov show ./jsonquery_test \
    -instr-profile=jsonquery_test.profdata  \
    -format=html \
    -output-dir=test-coverage \
    -Xdemangler=c++filt -Xdemangler=-n


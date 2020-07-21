#!/usr/bin/env bash

if [[ $# -lt 1 ]]; then
    echo "Usage: ./new_cmake.sh <build-type> [c-compiler] [cxx-compiler]"
    echo
    echo "<build-type>      Debug/Release"
    echo "[c-compiler]"
    echo "[cxx-compiler]    path to c/cxx compiler defaults to gcc/g++"
    exit 1
fi

BUILD_TYPE=$1
C=${2:-gcc}
CXX=${3:-g++}

cmake .. \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_C_COMPILER="$C" -DCMAKE_CXX_COMPILER="$CXX"


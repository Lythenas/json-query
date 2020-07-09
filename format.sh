#!/usr/bin/env bash

clang-format {src,test,bench}/**/*.{c,h}pp -i -style="{BasedOnStyle: google, IndentWidth: 4}"

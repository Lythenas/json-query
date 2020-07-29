# json-query

This is part of an assignment for the Object Oriented C++ course in Uni.

This project is more or less a clone of [jql](https://github.com/yamafaktory/jql)
but written in C++ instead of Rust.

It's a cli query tool for json files.
See [jql](https://github.com/yamafaktory/jql#%EF%B8%8F-usage) for examples.

## Building

```
cd build
./new_cmake.sh Debug # or Release (and optionally specify the c and c++ compiler)
make
```

## Running

```
cd build
make jsonquery_run
./src/jsonquery_run --help
```

## Tests

```
cd build
make jsonquery_test
./test/jsonquery_test
```

## Benchmarks

(incomplete)

```
cd build
make jsonquery_bench
./bench/jsonquery_bench
```


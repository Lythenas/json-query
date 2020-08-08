# json-query

This is part of an assignment for the Object Oriented C++ course in Uni.

This project is more or less a clone of [jql](https://github.com/yamafaktory/jql)
but written in C++ instead of Rust.

It's a cli query tool for json files.
See [jql](https://github.com/yamafaktory/jql#%EF%B8%8F-usage) for examples.

## Building

The sources are built with C++20 using the `Makefile`.

```
make
```

## Running

```
make jsonquery
./jsonquery --help
```

## Tests

```sh
make check
```

or

```sh
make jsonquery_test
./jsonquery_test
```

To execute tests with coverage use:

```sh
./coverage.sh
```

This will generate a coverage report using the clang/llvm tools in
`test-coverage/index.html`.

There is also a fuzzing test:

```sh
make clean
make FUZZ=1 jsonquery_fuzz
./jsonquery_fuzz
```

## Benchmarks

(incomplete)

## Dependencies

- boost (tested with version 1.72)

For testing:

- [Catch2](https://github.com/catchorg/Catch2) (header only, included in `test/` directory)

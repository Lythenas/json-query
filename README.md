# json-query

This is part of an assignment for the Object Oriented C++ course in Uni.

This project is more or less a clone of [jql](https://github.com/yamafaktory/jql)
but written in C++ instead of Rust.

It's a cli query tool for json files.
See [jql](https://github.com/yamafaktory/jql#%EF%B8%8F-usage) for examples.

## Building

```
make
```

## Running

```
make jsonquery
./jsonquery --help
```

## Tests

```
make check
```

or

```
make jsonquery_test
./jsonquery_test
```

There is also a fuzzing test

```
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

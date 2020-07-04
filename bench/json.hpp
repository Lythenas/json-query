#include <string>
#include <benchmark/benchmark.h>

#include "json/json.hpp"


static void BM_JsonParsing_SimpleObject(benchmark::State& state) {
    std::string json = R"#({"int": 5, "float": 22.5e100, "string": "something", "array": [1, 2, 3]})#";
    for (auto _ : state) {
        parse_json(json.begin(), json.end());
    }
}
static void BM_JsonParsing_SmallArray(benchmark::State& state) {
    std::string json = R"#([1, 2, 3, 4, 5, 6])#";
    for (auto _ : state) {
        parse_json(json.begin(), json.end());
    }
}
static void BM_JsonParsing_BigArray(benchmark::State& state) {
    std::string json = R"#([1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0])#";
    for (auto _ : state) {
        parse_json(json.begin(), json.end());
    }
}
BENCHMARK(BM_JsonParsing_SimpleObject);
BENCHMARK(BM_JsonParsing_SmallArray);
BENCHMARK(BM_JsonParsing_BigArray);

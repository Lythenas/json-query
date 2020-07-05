#include <benchmark/benchmark.h>

#include <string>

#include "json/json.hpp"

using namespace json;

static void BM_JsonParsing_Num_Array(benchmark::State& state) {
    std::string json = "[ ";
    auto sep = "";
    for (int i{0}; i < state.range(0); ++i) {
        json += sep;
        json += "9999";
        sep = ", ";
    }
    json += " ]";

    for (auto _ : state) {
        benchmark::DoNotOptimize(parse_json(json.begin(), json.end()));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_JsonParsing_Num_Array)
    ->RangeMultiplier(2)
    ->Range(1 << 4, 1 << 18)
    ->Complexity();

static void BM_JsonParsing_SimpleObject(benchmark::State& state) {
    std::string json =
        R"#({"int": 5, "float": 22.5e100, "string": "something", "array": [1, 2, 3]})#";
    for (auto _ : state) {
        parse_json(json.begin(), json.end());
    }
}
BENCHMARK(BM_JsonParsing_SimpleObject);

static void BM_JsonParsing_SmallArray(benchmark::State& state) {
    std::string json = R"#([1, 2, 3, 4, 5, 6])#";
    for (auto _ : state) {
        parse_json(json.begin(), json.end());
    }
}
BENCHMARK(BM_JsonParsing_SmallArray);

static void BM_JsonParsing_BigArray(benchmark::State& state) {
    std::string json =
        R"#([1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0])#";
    for (auto _ : state) {
        parse_json(json.begin(), json.end());
    }
}
BENCHMARK(BM_JsonParsing_BigArray);

#include <benchmark/benchmark.h>

#include <string>

#include "selectors/selectors.hpp"

using namespace selectors;

static void BM_SelectorParsing_Keys_Roots(benchmark::State& state) {
    const auto* const one_key = "\"some_key\"";
    std::string selector;
    const auto* sep = "";
    for (int i{0}; i < state.range(0); ++i) {
        selector += sep;
        selector += one_key;
        sep = ",";
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            parse_selectors(selector.begin(), selector.end()));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_SelectorParsing_Keys_Roots)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 18)
    ->Complexity();

static void BM_SelectorParsing_Keys_Consecutive(benchmark::State& state) {
    const auto* const one_key = "\"some_key\"";
    std::string selector;
    const auto* sep = "";
    for (int i{0}; i < state.range(0); ++i) {
        selector += sep;
        selector += one_key;
        sep = ".";
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            parse_selectors(selector.begin(), selector.end()));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_SelectorParsing_Keys_Consecutive)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 18)
    ->Complexity();

static void BM_SelectorParsing_Multiple2(benchmark::State& state) {
    std::string selector = "\"laptops\"|\"laptop\",\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
BENCHMARK(BM_SelectorParsing_Multiple2);

static void BM_SelectorParsing_Multiple8(benchmark::State& state) {
    std::string selector =
        "\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\","
        "\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|"
        "\"laptop\",\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
BENCHMARK(BM_SelectorParsing_Multiple8);

static void BM_SelectorParsing_Multiple32(benchmark::State& state) {
    std::string selector =
        "\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\","
        "\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|"
        "\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],"
        "\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\","
        "\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|"
        "\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],"
        "\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\","
        "\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|"
        "\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],"
        "\"laptops\"|\"laptop\",\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
BENCHMARK(BM_SelectorParsing_Multiple32);

static void BM_SelectorParsing_Single(benchmark::State& state) {
    std::string selector = "\"laptops\"|\"laptop\".\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
BENCHMARK(BM_SelectorParsing_Single);

static void BM_SelectorParsing_Single4(benchmark::State& state) {
    std::string selector =
        "\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\"."
        "\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|"
        "\"laptop\".\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
BENCHMARK(BM_SelectorParsing_Single4);

static void BM_SelectorParsing_Single16(benchmark::State& state) {
    std::string selector =
        "\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\"."
        "\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|"
        "\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0]."
        "\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\"."
        "\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|"
        "\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0]."
        "\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\"."
        "\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|"
        "\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0]."
        "\"laptops\"|\"laptop\".\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}

BENCHMARK(BM_SelectorParsing_Single16);

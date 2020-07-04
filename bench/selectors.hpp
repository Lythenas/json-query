#include <string>
#include <benchmark/benchmark.h>

#include "selectors/selectors.hpp"

static void BM_SelectorParsing_Multiple2(benchmark::State& state) {
    std::string selector = "\"laptops\"|\"laptop\",\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
static void BM_SelectorParsing_Multiple8(benchmark::State& state) {
    std::string selector =
        "\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|\"laptop\","
        "\"laptops\"[0],\"laptops\"|\"laptop\",\"laptops\"[0],\"laptops\"|"
        "\"laptop\",\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
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
static void BM_SelectorParsing_Single(benchmark::State& state) {
    std::string selector = "\"laptops\"|\"laptop\".\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
static void BM_SelectorParsing_Single4(benchmark::State& state) {
    std::string selector =
        "\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|\"laptop\"."
        "\"laptops\"[0].\"laptops\"|\"laptop\".\"laptops\"[0].\"laptops\"|"
        "\"laptop\".\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}
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

BENCHMARK(BM_SelectorParsing_Multiple2);
BENCHMARK(BM_SelectorParsing_Multiple8);
BENCHMARK(BM_SelectorParsing_Multiple32);
BENCHMARK(BM_SelectorParsing_Single);
BENCHMARK(BM_SelectorParsing_Single4);
BENCHMARK(BM_SelectorParsing_Single16);

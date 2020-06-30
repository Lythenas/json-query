#include <benchmark/benchmark.h>
#include <string>
#include "selectors/selectors.hpp"

static void BM_SelectorParsing(benchmark::State& state) {
    std::string selector = "\"laptops\"|\"laptop\",\"laptops\"[0]";
    for (auto _ : state) {
        parse_selectors(selector.begin(), selector.end());
    }
}

BENCHMARK(BM_SelectorParsing);

BENCHMARK_MAIN();

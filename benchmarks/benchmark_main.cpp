#define ANKERL_NANOBENCH_IMPLEMENTATION
#include "../libs/nanobench.h"
#include "benchmark_json.h"
#include "benchmark_datamanager.h"
#include "benchmark_sorting.h"

#include <iostream>

int main() {
    std::cout << "======================================\n";
    std::cout << "     RUNNING ISOLATED BENCHMARKS     \n";
    std::cout << "======================================\n\n";

    ankerl::nanobench::Bench bench;
    bench.title("Lab 3 Performance")
         .unit("ms")
         .minEpochIterations(5);

    BenchmarkJson::run(bench);
    BenchmarkDataManager::run(bench);
    BenchmarkSorting::run(bench);

    return 0;
}
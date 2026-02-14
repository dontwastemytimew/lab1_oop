#define ANKERL_NANOBENCH_IMPLEMENTATION
#include "nanobench.h"
#include "benchmark_json.h"
#include "benchmark_repository.h"
#include "benchmark_sorting.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    std::cout << "======================================\n";
    std::cout << "   BENCHMARKING REFACTORED SYSTEM    \n";
    std::cout << "======================================\n\n";

    ankerl::nanobench::Bench bench;
    bench.title("Refactoring Performance Impact")
         .unit("ms")
         .relative(true)
         .minEpochIterations(5);

    BenchmarkJson::run(bench);

    BenchmarkRepository::run(bench);

    BenchmarkSorting::run(bench);

    return 0;
}
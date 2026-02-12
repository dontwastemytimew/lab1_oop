#ifndef BENCHMARK_SORTING_H
#define BENCHMARK_SORTING_H

#include "nanobench.h"
#include "datamanager.h"

class BenchmarkSorting {
public:
    static void run(ankerl::nanobench::Bench& bench) {
        DataManager dm;
        int count = 20000;
        for(int i=0; i<count; ++i) {
            Note n("Note " + QString::number(count - i), 0);
            if (i % 10 == 0) n.setPinned(true);
            dm.addNote(n);
        }

        bench.run("Sorting: ByNameAZ (20k Notes)", [&] {
            dm.sortNotes(SortType::ByNameAZ);
            ankerl::nanobench::doNotOptimizeAway(dm.getNotes());
        });
    }
};

#endif //BENCHMARK_SORTING_H

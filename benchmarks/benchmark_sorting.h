#ifndef BENCHMARK_SORTING_H
#define BENCHMARK_SORTING_H

#include "nanobench.h"
#include "note_repository.h"
#include "json_storage_strategy.h"

class BenchmarkSorting {
public:
    static void run(ankerl::nanobench::Bench& bench) {
        NoteRepository repo(std::make_unique<JsonStorageStrategy>());

        int count = 20000;
        for(int i=0; i<count; ++i) {
            Note n("Note " + QString::number(count - i), 0);
            if (i % 10 == 0) n.setPinned(true);
            repo.addNote(n);
        }

        bench.run("Sorting: ByNameAZ (20k Notes)", [&] {
            repo.sortNotes(SortType::ByNameAZ);
            ankerl::nanobench::doNotOptimizeAway(repo.getNotes());
        });
    }
};

#endif
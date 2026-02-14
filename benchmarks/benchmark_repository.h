#ifndef BENCHMARK_REPOSITORY_H
#define BENCHMARK_REPOSITORY_H

#include "nanobench.h"
#include "note_repository.h"
#include "json_storage_strategy.h"
#include "rapidjson_storage_strategy.h"
#include <QFile>

class BenchmarkRepository {
public:
    static void run(ankerl::nanobench::Bench& bench) {
        auto repo = std::make_unique<NoteRepository>(std::make_unique<JsonStorageStrategy>());

        int count = 10000;
        for(int i=0; i<count; ++i) {
            Note n("Note " + QString::number(i), 0);
            n.addField("Data", "Content " + QString::number(i));
            repo->addNote(n);
        }

        QString filename_json = "bench_nlohmann.json";
        QString filename_rapid = "bench_rapid.json";

        bench.run("Repository: Save (Nlohmann Strategy)", [&] {
            repo->save(filename_json);
        });

        repo->setStrategy(std::make_unique<RapidJsonStorageStrategy>());
        bench.run("Repository: Save (RapidJSON Strategy)", [&] {
            repo->save(filename_rapid);
        });

        repo->setStrategy(std::make_unique<JsonStorageStrategy>());
        bench.run("Repository: Load (Nlohmann Strategy)", [&] {
            repo->load(filename_json);
            ankerl::nanobench::doNotOptimizeAway(repo->getNotes());
        });

        QFile::remove(filename_json);
        QFile::remove(filename_rapid);
    }
};

#endif
#ifndef BENCHMARK_DATAMANAGER_H
#define BENCHMARK_DATAMANAGER_H

#include "nanobench.h"
#include "datamanager.h"
#include <QFile>

class BenchmarkDataManager {
public:
    static void run(ankerl::nanobench::Bench& bench) {
        DataManager dm;
        Schema schema("BenchmarkSchema");
        schema.addField({"Data", "Text"});
        dm.addSchema(schema);

        int count = 10000;
        for(int i=0; i<count; ++i) {
            Note n("Note " + QString::number(i), 0);
            n.addField("Data", "Some content " + QString::number(i));
            if (i % 100 == 0) n.addTag("tag");
            dm.addNote(n);
        }

        QString filename = "bench_temp.json";

        bench.run("DataManager: Save (10k Notes)", [&] {
            dm.saveToFile(filename);
        });

        bench.run("DataManager: Load (10k Notes)", [&] {
            DataManager dm_load;
            dm_load.loadFromFile(filename);
            ankerl::nanobench::doNotOptimizeAway(dm_load);
        });

        QFile::remove(filename);
    }
};

#endif //BENCHMARK_DATAMANAGER_H

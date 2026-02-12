#ifndef BENCHMARK_JSON_H
#define BENCHMARK_JSON_H

#include "nanobench.h"
#include "json.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <vector>
#include <string>

using nlohmann_json = nlohmann::json;

class BenchmarkJson {
public:
    static void run(ankerl::nanobench::Bench& bench) {
        int count = 50000;
        std::vector<std::string> testData;
        testData.reserve(count);
        for(int i = 0; i < count; ++i) testData.push_back("Data " + std::to_string(i));

        bench.run("Library: nlohmann/json (Serialize)", [&] {
            nlohmann_json root;
            root["list"] = nlohmann_json::array();
            for(const auto& str : testData) {
                root["list"].push_back(str);
            }
            auto res = root.dump();
            ankerl::nanobench::doNotOptimizeAway(res);
        });

        bench.run("Library: RapidJSON (Serialize)", [&] {
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);
            writer.StartObject();
            writer.Key("list");
            writer.StartArray();
            for(const auto& str : testData) {
                writer.String(str.c_str());
            }
            writer.EndArray();
            writer.EndObject();
            auto res = s.GetString();
            ankerl::nanobench::doNotOptimizeAway(res);
        });
    }
};

#endif //BENCHMARK_JSON_H

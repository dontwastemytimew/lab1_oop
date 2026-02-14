#pragma once
#include "istorage_strategy.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <QFile>
#include <QDebug>
#include <fstream>
#include <sstream>

class RapidJsonStorageStrategy : public IStorageStrategy {
public:
    bool save(const QString& filePath,
              const QList<Note>& notes,
              const QList<Schema>& schemas,
              const QMap<QString, int>& stats) override
    {
        using namespace rapidjson;
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);

        writer.StartObject();

        writer.Key("schemas");
        writer.StartArray();
        for (const auto& schema : schemas) {
            writer.StartObject();
            writer.Key("name");
            writer.String(schema.getName().toStdString().c_str());

            writer.Key("fields");
            writer.StartArray();
            for (const auto& field : schema.getFields()) {
                writer.StartObject();
                writer.Key("name"); writer.String(field.name.toStdString().c_str());
                writer.Key("type"); writer.String(field.type.toStdString().c_str());
                writer.EndObject();
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("notes");
        writer.StartArray();
        for (const auto& note : notes) {
            writer.StartObject();
            writer.Key("title"); writer.String(note.getTitle().toStdString().c_str());
            writer.Key("schemaId"); writer.Int(note.getSchemaId());

            writer.Key("fields");
            writer.StartObject();
            auto fields = note.getFields();
            for (auto it = fields.begin(); it != fields.end(); ++it) {
                writer.Key(it.key().toStdString().c_str());
                writer.String(it.value().toStdString().c_str());
            }
            writer.EndObject();

            writer.Key("tags");
            writer.StartArray();
            for (const auto& tag : note.getTags()) {
                writer.String(tag.toStdString().c_str());
            }
            writer.EndArray();

            writer.Key("image"); writer.String(note.getImage().toStdString().c_str());
            writer.Key("pinned"); writer.Bool(note.isPinned());

            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("stats");
        writer.StartObject();
        for(auto it = stats.begin(); it != stats.end(); ++it) {
            writer.Key(it.key().toStdString().c_str());
            writer.Int(it.value());
        }
        writer.EndObject();

        writer.EndObject();

        std::ofstream file(filePath.toStdString());
        if (!file.is_open()) return false;

        file << sb.GetString();
        qInfo() << "Saved via RapidJSON Strategy";
        return true;
    }

    bool load(const QString& filePath,
              QList<Note>& notes,
              QList<Schema>& schemas,
              QMap<QString, int>& stats) override
    {

        qWarning() << "RapidJSON load not implemented for full structure yet.";
        return false;
    }
};
#pragma once
#include "istorage_strategy.h"
#include "json.hpp"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QObject>

using json = nlohmann::json;

/**
 * @class JsonStorageStrategy
 * @brief Реалізація стратегії збереження за допомогою бібліотеки nlohmann/json.
 * * @details Це стандартний механізм збереження для програми. Він забезпечує
 * повну сумісність з форматом JSON, що використовувався у попередніх версіях.
 */
class JsonStorageStrategy : public IStorageStrategy {
public:
    bool save(const QString& filePath,
              const QList<Note>& notes,
              const QList<Schema>& schemas,
              const QMap<QString, int>& stats) override
    {
        json rootObject;

        rootObject["schemas"] = json::array();
        for (const auto& schema : schemas) {
            json schemaObject;
            schemaObject["name"] = schema.getName().toStdString();
            schemaObject["fields"] = json::array();
            for (const auto& field : schema.getFields()) {
                json fieldObject;
                fieldObject["name"] = field.name.toStdString();
                fieldObject["type"] = field.type.toStdString();
                schemaObject["fields"].push_back(fieldObject);
            }
            rootObject["schemas"].push_back(schemaObject);
        }

        rootObject["notes"] = json::array();
        for (const auto& note : notes) {
            json noteObject;
            noteObject["title"] = note.getTitle().toStdString();
            noteObject["schemaId"] = note.getSchemaId();

            noteObject["fields"] = json::object();
            const auto& fields = note.getFields();
            for (auto it = fields.constBegin(); it != fields.constEnd(); ++it) {
                noteObject["fields"][it.key().toStdString()] = it.value().toStdString();
            }

            noteObject["tags"] = json::array();
            for (const auto& tag : note.getTags()) {
                noteObject["tags"].push_back(tag.toStdString());
            }

            noteObject["image"] = note.getImage().toStdString();
            noteObject["pinned"] = note.isPinned();

            rootObject["notes"].push_back(noteObject);
        }

        rootObject["stats"] = json::object();
        for(auto it = stats.begin(); it != stats.end(); ++it) {
            rootObject["stats"][it.key().toStdString()] = it.value();
        }

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << QString::fromStdString(rootObject.dump(4));
            file.close();
            qInfo() << QObject::tr("Дані успішно збережено (Nlohmann):") << filePath;
            return true;
        } else {
            qCritical() << QObject::tr("ПОМИЛКА: не вдалося відкрити файл для збереження:") << filePath;
            return false;
        }
    }

    bool load(const QString& filePath,
              QList<Note>& notes,
              QList<Schema>& schemas,
              QMap<QString, int>& stats) override
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << QObject::tr("Файл даних не знайдено. Буде створено новий.");
            return false;
        }

        QString content = file.readAll();
        file.close();

        json rootObject;
        try {
            rootObject = json::parse(content.toStdString());
        } catch (const std::exception& e) {
            qCritical() << QObject::tr("ПОМИЛКА: Не вдалося розпарсити JSON:") << e.what();
            return false;
        }

        schemas.clear();
        if (rootObject.contains("schemas")) {
            for (const auto& schemaObject : rootObject["schemas"]) {
                Schema newSchema(QString::fromStdString(schemaObject["name"]));
                if (schemaObject.contains("fields")) {
                    for (const auto& fieldObject : schemaObject["fields"]) {
                        newSchema.addField({
                            QString::fromStdString(fieldObject["name"]),
                            QString::fromStdString(fieldObject["type"])
                        });
                    }
                }
                schemas.append(newSchema);
            }
        }

        notes.clear();
        if (rootObject.contains("notes")) {
            for (const auto& noteObject : rootObject["notes"]) {
                Note newNote(
                    QString::fromStdString(noteObject["title"]),
                    noteObject["schemaId"]
                );

                if (noteObject.contains("fields")) {
                    for (auto& [key, value] : noteObject["fields"].items()) {
                        newNote.addField(QString::fromStdString(key), QString::fromStdString(value));
                    }
                }

                if (noteObject.contains("tags")) {
                    for (const auto& tag : noteObject["tags"]) {
                        newNote.addTag(QString::fromStdString(tag));
                    }
                }

                if (noteObject.contains("image")) {
                    newNote.setImage(QString::fromStdString(noteObject["image"]));
                }

                newNote.setPinned(noteObject.value("pinned", false));
                notes.append(newNote);
            }
        }

        stats.clear();
        if (rootObject.contains("stats")) {
            for (auto& [key, value] : rootObject["stats"].items()) {
                stats[QString::fromStdString(key)] = value;
            }
        }

        return true;
    }
};
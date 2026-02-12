#include "datamanager.h"
#include "json.hpp"
#include <QList>
#include <QString>
#include <QDebug>
#include <QObject>
#include <QFile>
#include <QPrinter>
#include <QTextDocument>
#include <QDateTime>
#include <algorithm>
#include <QTextStream>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <fstream>

using json = nlohmann::json;

DataManager::DataManager() {
    Schema bookSchema(QObject::tr("Книга"));
    bookSchema.addField({QObject::tr("Автор"), QObject::tr("Текст")});
    bookSchema.addField({QObject::tr("Рік видання"), QObject::tr("Число")});
    m_schemas.append(bookSchema);
}

void DataManager::addSchema(const Schema &schema) {
    m_schemas.append(schema);
}

const QList<Schema>& DataManager::getSchemas() const {
    return m_schemas;
}

void DataManager::removeSchema(int index) {
    if (index >= 0 && index < m_schemas.size()) {
        m_schemas.removeAt(index);
    }
}

void DataManager::updateSchema(int index, const Schema& schema) {
    if (index >= 0 && index < m_schemas.size()) {
        m_schemas[index] = schema;
    }
}

void DataManager::addNote(const Note &note) {
    m_notes.append(note);
}

QList<Note>& DataManager::getNotes() {
    return m_notes;
}

void DataManager::updateNote(int index, const Note& note) {
    if (index >= 0 && index < m_notes.size()) {
        m_notes[index] = note;
    }
}

void DataManager::saveToFile(const QString &filePath) const {
    json rootObject;

    rootObject["schemas"] = json::array();
    for (const auto& schema : m_schemas) {
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
    for (const auto& note : m_notes) {
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
    for(auto it = m_usageStats.begin(); it != m_usageStats.end(); ++it) {
        rootObject["stats"][it.key().toStdString()] = it.value();
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << QString::fromStdString(rootObject.dump(4));
        file.close();
        qInfo() << QObject::tr("Дані успішно збережено у файл:") << filePath;
    } else {
        qCritical() << QObject::tr("ПОМИЛКА: не вдалося відкрити файл для збереження:") << filePath;
    }
}

void DataManager::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << QObject::tr("Файл даних не знайдено (або неможливо відкрити). Буде створено новий.");
        return;
    }

    QString content = file.readAll();
    file.close();

    json rootObject;
    try {
        rootObject = json::parse(content.toStdString());
    } catch (const std::exception& e) {
        qCritical() << QObject::tr("ПОМИЛКА: Не вдалося розпарсити JSON файл:") << filePath << e.what();
        return;
    }

    m_schemas.clear();
    m_notes.clear();
    m_usageStats.clear();

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
            m_schemas.append(newSchema);
        }
    }

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

            m_notes.append(newNote);
        }
    }

    if (rootObject.contains("stats")) {
        for (auto& [key, value] : rootObject["stats"].items()) {
            m_usageStats[QString::fromStdString(key)] = value;
        }
    }

        // ГЕНЕРАЦІЯ ФЕЙКОВОЇ СТАТИСТИКИ (Якщо її немає)
        // Це створить дані за останні 7 днів
        if (m_usageStats.isEmpty()) {
            QDate today = QDate::currentDate();
            for (int i = 6; i >= 0; --i) {
                QDate date = today.addDays(-i);
                int minutes = QRandomGenerator::global()->bounded(5, 60);
                m_usageStats[date.toString("yyyy-MM-dd")] = minutes * 60;
            }
        }

    qInfo() << QObject::tr("Дані успішно завантажено. Знайдено %1 схем та %2 нотаток.")
               .arg(m_schemas.size()).arg(m_notes.size());
}

void DataManager::removeNote(int index) {
    if (index >= 0 && index < m_notes.size()) {
        m_notes.removeAt(index);
    }
}

void DataManager::exportNote(int index, const QString& filePath) const {
    if (index < 0 || index >= m_notes.size()) return;

    json noteObject;
    const Note& note = m_notes[index];

    noteObject["title"] = note.getTitle().toStdString();
    noteObject["schemaId"] = note.getSchemaId();

    noteObject["tags"] = json::array();
    for (const auto& tag : note.getTags()) {
        noteObject["tags"].push_back(tag.toStdString());
    }

    noteObject["fields"] = json::object();
    for (auto it = note.getFields().constBegin(); it != note.getFields().constEnd(); ++it) {
        noteObject["fields"][it.key().toStdString()] = it.value().toStdString();
    }

    noteObject["image"] = note.getImage().toStdString();

    noteObject["pinned"] = note.isPinned();

    std::ofstream file(filePath.toStdString());
    if (file.is_open()) {
        file << noteObject.dump(4);
        qInfo() << QObject::tr("Нотатку '%1' успішно експортовано у файл: %2").arg(note.getTitle()).arg(filePath);
    } else {
        qCritical() << QObject::tr("ПОМИЛКА: Не вдалося відкрити файл для експорту:") << filePath;
    }
}


void DataManager::importNote(const QString& filePath) {
    qDebug() << "--- Початок імпорту з файлу:" << filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << QObject::tr("ПОМИЛКА: Не вдалося відкрити файл для імпорту: %1").arg(filePath);
        return;
    }

    QString content = file.readAll();
    file.close();

    json noteObject;
    try {
        noteObject = json::parse(content.toStdString());
        qDebug() << "JSON файл успішно розпарсено.";
    } catch (const std::exception& e) {
        qCritical() << QObject::tr("ПОМИЛКА: JSON файл пошкоджено! %1").arg(e.what());
        return;
    }

    QString title = QString::fromStdString(noteObject.value("title", QObject::tr("Імпортована нотатка").toStdString()));
    int schemaId = noteObject.value("schemaId", 0);

    Note importedNote(title, schemaId);

    if (noteObject.contains("fields")) {
        for (auto& [key, value] : noteObject["fields"].items()) {
            importedNote.addField(QString::fromStdString(key), QString::fromStdString(value));
        }
    }

    if (noteObject.contains("tags")) {
        QSet<QString> tags;
        for (const auto& tag : noteObject["tags"]) {
            tags.insert(QString::fromStdString(tag));
        }
        importedNote.setTags(tags);
    }

    if (noteObject.contains("image")) {
        importedNote.setImage(QString::fromStdString(noteObject["image"]));
    }

    importedNote.setPinned(noteObject.value("pinned", false));

    m_notes.append(importedNote);
    qInfo() << QObject::tr("Нотатку успішно імпортовано з файлу: %1").arg(filePath);
}

void DataManager::exportNoteToPdf(int index, const QString& filePath) const {
    if (index < 0 || index >= m_notes.size()) return;

    const Note& note = m_notes[index];

    QString html = "<html><body>";

    html += QString("<h1 align='center'>%1</h1>").arg(note.getTitle());

    html += QString("<p align='right'><i>Date: %1</i></p>").arg(note.getCreationDate().toString("dd.MM.yyyy HH:mm"));

    if (!note.getTags().isEmpty()) {
        QStringList tagsList = note.getTags().values();
        html += QString("<p><b>Tags:</b> <span style='color: gray'>#%1</span></p>").arg(tagsList.join(" #"));
    }

    html += "<hr>";

    if (!note.getImage().isEmpty()) {
        html += QString("<div align='center'><img src='data:image/png;base64,%1' width='400'></div><br>").arg(note.getImage());
    }

    if (!note.getFields().isEmpty()) {
        html += "<table border='1' cellspacing='0' cellpadding='5' width='100%'>";
        for (auto it = note.getFields().constBegin(); it != note.getFields().constEnd(); ++it) {
            html += QString("<tr><td bgcolor='#f0f0f0' width='30%'><b>%1</b></td><td>%2</td></tr>")
                    .arg(it.key())
                    .arg(it.value());
        }
        html += "</table>";
    }

    html += "</body></html>";

    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);

    qInfo() << QObject::tr("Нотатку успішно експортовано в PDF: %1").arg(filePath);
}

void DataManager::sortNotes(SortType type) {
    std::stable_sort(m_notes.begin(), m_notes.end(), [type](const Note& a, const Note& b) {

            if (a.isPinned() && !b.isPinned()) {
                 return true;
             }
             if (!a.isPinned() && b.isPinned()) {
                 return false;
             }

        switch (type) {
            case SortType::ByDateNewest:
                return a.getCreationDate() > b.getCreationDate();
            case SortType::ByDateOldest:
                return a.getCreationDate() < b.getCreationDate();
            case SortType::ByNameAZ:
                return a.getTitle().localeAwareCompare(b.getTitle()) < 0;
            case SortType::ByNameZA:
                return a.getTitle().localeAwareCompare(b.getTitle()) > 0;
            default:
                return false;
        }
    });
}

void DataManager::addUsageTime(int seconds) {
    QString today = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    m_usageStats[today] += seconds;
}

QMap<QString, int> DataManager::getUsageStats() const {
    return m_usageStats;
}

QList<Note> DataManager::generateTestNotes(int count) const {
    if (m_schemas.isEmpty()) return {};

    const Schema& baseSchema = m_schemas.first();
    const int schemaId = 0;

    QList<Note> testNotes;

    for (int i = 0; i < count; ++i) {
        QString title = QString("Test Note %1").arg(i + 1);

        Note note(title, schemaId);

        for (const auto& field : baseSchema.getFields()) {
            QString value;
            if (field.type == QObject::tr("Число")) {
                value = QString::number(QRandomGenerator::global()->bounded(1900, 2024));
            } else {
                value = QString("Value %1 for %2").arg(i).arg(field.name);
            }
            note.addField(field.name, value);
        }

        if (i % 10 == 0) {
            note.addTag("benchmark");
        }
        if (i % 5 == 0) {
            note.addTag("test");
        }

        if (i % 20 == 0) {
            note.setImage("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8BQDwAEhQGAhKmMIQAAAABJRU5ErkJggg==");
        }

        testNotes.append(note);
    }
    return testNotes;
}

QPair<qint64, qint64> DataManager::runSystemBenchmark(int noteCount) {
    if (m_schemas.isEmpty()) {
        Schema defaultSchema("DefaultTest");
        defaultSchema.addField({"TextValue", "Текст"});
        m_schemas.append(defaultSchema);
    }

    QList<Note> originalNotes = m_notes;
    QList<Schema> originalSchemas = m_schemas;

    QList<Note> testNotes = generateTestNotes(noteCount);

    m_notes = testNotes;

    QString tempFilePath = "benchmark_temp_data.json";

    QElapsedTimer saveTimer;
    saveTimer.start();
    saveToFile(tempFilePath);
    qint64 saveTime = saveTimer.elapsed();

    m_notes.clear();
    m_schemas.clear();

    QElapsedTimer loadTimer;
    loadTimer.start();
    loadFromFile(tempFilePath);
    qint64 loadTime = loadTimer.elapsed();

    QFile::remove(tempFilePath);

    m_notes = originalNotes;
    m_schemas = originalSchemas;

    return qMakePair(saveTime, loadTime);
}
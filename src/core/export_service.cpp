#include "export_service.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPrinter>
#include <QTextDocument>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

bool ExportService::exportToJson(const Note& note, const QString& filePath) {
    json noteObject;

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
        qInfo() << "ExportService: JSON exported to" << filePath;
        return true;
    } else {
        qCritical() << "ExportService Error: Cannot open file" << filePath;
        return false;
    }
}

bool ExportService::exportToPdf(const Note& note, const QString& filePath) {
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
    qInfo() << "ExportService: PDF exported to" << filePath;
    return true;
}

std::optional<Note> ExportService::importFromJson(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return std::nullopt;

    nlohmann::json obj;
    try {
        obj = nlohmann::json::parse(file.readAll().toStdString());

        QString title = QString::fromStdString(obj.value("title", "Imported Note"));
        int schemaId = obj.value("schemaId", 0);

        Note note(title, schemaId);

        if (obj.contains("fields")) {
            for (auto& [key, value] : obj["fields"].items()) {
                note.addField(QString::fromStdString(key), QString::fromStdString(value));
            }
        }

        if (obj.contains("tags")) {
            QSet<QString> tags;
            for (const auto& tag : obj["tags"]) tags.insert(QString::fromStdString(tag));
            note.setTags(tags);
        }

        note.setImage(QString::fromStdString(obj.value("image", "")));
        note.setPinned(obj.value("pinned", false));

        return note;
    } catch (...) {
        return std::nullopt;
    }
}
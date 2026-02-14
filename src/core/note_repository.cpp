#include "note_repository.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPrinter>
#include <QTextDocument>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

NoteRepository::NoteRepository(std::unique_ptr<IStorageStrategy> strategy)
    : m_strategy(std::move(strategy))
{
}

void NoteRepository::addSchema(const Schema& schema) {
    m_schemas.append(schema);
}

const QList<Schema>& NoteRepository::getSchemas() const {
    return m_schemas;
}

void NoteRepository::removeSchema(int index) {
    if (index >= 0 && index < m_schemas.size()) {
        m_schemas.removeAt(index);
    }
}

void NoteRepository::updateSchema(int index, const Schema& schema) {
    if (index >= 0 && index < m_schemas.size()) {
        m_schemas[index] = schema;
    }
}

void NoteRepository::addNote(const Note& note) {
    m_notes.append(note);
}

QList<Note>& NoteRepository::getNotes() {
    return m_notes;
}

void NoteRepository::removeNote(int index) {
    if (index >= 0 && index < m_notes.size()) {
        m_notes.removeAt(index);
    }
}

void NoteRepository::updateNote(int index, const Note& note) {
    if (index >= 0 && index < m_notes.size()) {
        m_notes[index] = note;
    }
}

void NoteRepository::sortNotes(SortType type) {
    std::stable_sort(m_notes.begin(), m_notes.end(), [type](const Note& a, const Note& b) {
        if (a.isPinned() && !b.isPinned()) return true;
        if (!a.isPinned() && b.isPinned()) return false;

        switch (type) {
            case SortType::ByDateNewest: return a.getCreationDate() > b.getCreationDate();
            case SortType::ByDateOldest: return a.getCreationDate() < b.getCreationDate();
            case SortType::ByNameAZ: return a.getTitle().localeAwareCompare(b.getTitle()) < 0;
            case SortType::ByNameZA: return a.getTitle().localeAwareCompare(b.getTitle()) > 0;
            default: return false;
        }
    });
}

void NoteRepository::addUsageTime(int seconds) {
    QString today = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    m_usageStats[today] += seconds;
}

QMap<QString, int> NoteRepository::getUsageStats() const {
    return m_usageStats;
}

bool NoteRepository::save(const QString& filePath) {
    if (!m_strategy) return false;
    return m_strategy->save(filePath, m_notes, m_schemas, m_usageStats);
}

bool NoteRepository::load(const QString& filePath) {
    if (!m_strategy) return false;

    bool success = m_strategy->load(filePath, m_notes, m_schemas, m_usageStats);

    if (m_schemas.isEmpty()) {
        Schema bookSchema(QObject::tr("Книга"));
        bookSchema.addField({QObject::tr("Автор"), QObject::tr("Текст")});
        bookSchema.addField({QObject::tr("Рік видання"), QObject::tr("Число")});
        m_schemas.append(bookSchema);
    }

    if (m_usageStats.isEmpty()) {
        QDate today = QDate::currentDate();
        for (int i = 6; i >= 0; --i) {
            QDate date = today.addDays(-i);
            int minutes = QRandomGenerator::global()->bounded(5, 60);
            m_usageStats[date.toString("yyyy-MM-dd")] = minutes * 60;
        }
    }

    return success;
}

void NoteRepository::setStrategy(std::unique_ptr<IStorageStrategy> newStrategy) {
    m_strategy = std::move(newStrategy);
}
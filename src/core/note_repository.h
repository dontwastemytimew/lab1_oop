#pragma once
#include <QList>
#include <QMap>
#include <memory>
#include "note.h"
#include "schema.h"
#include "istorage_strategy.h"
#include <algorithm>
#include <QObject>

enum class SortType {
    ByDateNewest,
    ByDateOldest,
    ByNameAZ,
    ByNameZA
};

/**
 * @class NoteRepository
 * @brief Центральний репозиторій для керування даними (Repository Pattern).
 *
 * @details Клас відповідає за CRUD операції над нотатками та схемами.
 * Він ізолює логіку збереження, делегуючи її об'єкту IStorageStrategy.
 * Також містить бізнес-логіку експорту та імпорту.
 */
class NoteRepository {
public:
    /**
     * @brief Конструктор. Приймає стратегію збереження.
     * @param strategy Унікальний вказівник на конкретну реалізацію збереження.
     */
    explicit NoteRepository(std::unique_ptr<IStorageStrategy> strategy);

    // Робота зі схемами
    void addSchema(const Schema& schema);
    const QList<Schema>& getSchemas() const;
    void removeSchema(int index);
    void updateSchema(int index, const Schema& schema);

    // Робота з нотатками
    void addNote(const Note& note);
    QList<Note>& getNotes();
    void removeNote(int index);
    void updateNote(int index, const Note& note);
    void sortNotes(SortType type);

    // Статистика
    void addUsageTime(int seconds);
    QMap<QString, int> getUsageStats() const;

    // Збереження/Завантаження
    bool save(const QString& filePath);
    bool load(const QString& filePath);

    /**
     * @brief Змінює стратегію збереження "на льоту".
     */
    void setStrategy(std::unique_ptr<IStorageStrategy> newStrategy);

private:
    QList<Schema> m_schemas;
    QList<Note> m_notes;
    QMap<QString, int> m_usageStats;
    std::unique_ptr<IStorageStrategy> m_strategy;
};
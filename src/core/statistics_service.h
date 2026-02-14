#pragma once
#include <QMap>
#include <QString>
#include <QDateTime>
#include "note_repository.h"

/**
 * @class StatisticsService
 * @brief Сервіс для аналізу та обробки статистики використання.
 */
class StatisticsService {
public:
    /**
     * @brief Розраховує загальну кількість нотаток за категоріями.
     */
    static QMap<QString, int> getNotesCountByCategory(NoteRepository* repo);

    /**
     * @brief Отримує форматовану статистику часу використання.
     */
    static QMap<QString, int> getUserActivity(NoteRepository* repo);
};
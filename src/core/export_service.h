#pragma once
#include <QString>
#include "note.h"

/**
 * @class ExportService
 * @brief Сервіс, відповідальний виключно за експорт даних у зовнішні формати.
 * * @details Клас приймає дані (нотатку)
 * і шлях до файлу, та виконує генерацію (JSON або PDF).
 */
class ExportService {
public:
    /**
     * @brief Експортує нотатку в JSON файл.
     */
    static bool exportToJson(const Note& note, const QString& filePath);

    /**
     * @brief Експортує нотатку в PDF документ з форматуванням.
     */
    static bool exportToPdf(const Note& note, const QString& filePath);

 /**
     * @brief Імпортує нотатку з JSON файлу.
     * @return std::optional<Note> Повертає об'єкт нотатки, якщо імпорт успішний.
     */
 static std::optional<Note> importFromJson(const QString& filePath);
};
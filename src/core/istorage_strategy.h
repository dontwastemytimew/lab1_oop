#pragma once
#include <QList>
#include <QString>
#include "note.h"
#include "schema.h"

/**
 * @interface IStorageStrategy
 * @brief Інтерфейс для реалізації патерну Стратегія (Strategy Pattern).
 *
 * @details Визначає абстракцію для механізмів збереження та завантаження даних.
 * Дозволяє динамічно змінювати формат зберігання (JSON, XML, SQL) без
 * модифікації основного коду програми.
 */
class IStorageStrategy {
public:
 virtual ~IStorageStrategy() = default;

 /**
  * @brief Зберігає дані у вказаний файл.
  * @param filePath Шлях до файлу.
  * @param notes Список нотаток для збереження.
  * @param schemas Список схем для збереження.
  * @param stats Статистика використання для збереження.
  * @return true, якщо операція успішна, інакше false.
  */
 virtual bool save(const QString& filePath,
                   const QList<Note>& notes,
                   const QList<Schema>& schemas,
                   const QMap<QString, int>& stats) = 0;

 /**
  * @brief Завантажує дані з файлу.
  * @param filePath Шлях до файлу.
  * @param notes [out] Посилання на список для заповнення нотатками.
  * @param schemas [out] Посилання на список для заповнення схемами.
  * @param stats [out] Посилання на мапу для заповнення статистикою.
  * @return true, якщо операція успішна, інакше false.
  */
 virtual bool load(const QString& filePath,
                   QList<Note>& notes,
                   QList<Schema>& schemas,
                   QMap<QString, int>& stats) = 0;
};
#pragma once
#include "schema.h"
#include <QList>
#include <QMap>
#include "note.h"

/**
 * @enum SortType
 * @brief Визначає доступні способи сортування списку нотаток.
 */
enum class SortType {
    ByDateNewest, ///< Сортування за датою створення (від нових до старих).
    ByDateOldest, ///< Сортування за датою створення (від старих до нових).
    ByNameAZ,     ///< Алфавітне сортування назви (А-Я).
    ByNameZA      ///< Зворотне алфавітне сортування назви (Я-А).
};

/**
 * @class DataManager
 * @brief Центральний клас для керування всіма даними програми.
 *
 * Відповідає за зберігання, завантаження, додавання, оновлення та видалення
 * схем і нотаток. Виступає як єдине джерело даних ("мозок") для всього GUI.
 * Також керує статистикою використання та експортом даних.
 */
class DataManager {
public:
    /**
     * @brief Конструктор. Створює одну тестову схему "Книга" для демонстрації.
     */
    DataManager();

    // --- Методи для роботи зі схемами ---

    /**
     * @brief Додає нову схему до сховища.
     * @param schema Об'єкт схеми для додавання.
     */
    void addSchema(const Schema& schema);

    /**
     * @brief Повертає список усіх схем.
     * @return Константне посилання на QList об'єктів Schema.
     */
    const QList<Schema>& getSchemas() const;

    /**
     * @brief Видаляє схему зі сховища за її індексом.
     * @param index Індекс схеми, яку потрібно видалити.
     */
    void removeSchema(int index);

    /**
     * @brief Оновлює існуючу схему за її індексом.
     * @param index Індекс схеми у списку, яку потрібно оновити.
     * @param schema Новий об'єкт схеми, який замінить старий.
     */
    void updateSchema(int index, const Schema& schema);

    // --- Методи для роботи з нотатками ---

    /**
     * @brief Додає нову нотатку до сховища.
     * @param note Об'єкт нотатки для додавання.
     */
    void addNote(const Note& note);

    /**
     * @brief Повертає список усіх нотаток для читання та зміни.
     * @return Посилання на QList об'єктів Note.
     */
    QList<Note>& getNotes();

    /**
     * @brief Видаляє нотатку зі сховища за її індексом.
     * @param index Індекс нотатки, яку потрібно видалити.
     */
    void removeNote(int index);

    /**
     * @brief Оновлює існуючу нотатку за її індексом.
     * @param index Індекс нотатки, яку потрібно оновити.
     * @param note Новий об'єкт нотатки для заміни.
     */
    void updateNote(int index, const Note& note);

    /**
     * @brief Сортує список нотаток відповідно до обраного типу.
     *
     * Закріплені нотатки (Pinned) завжди залишаються нагорі списку,
     * сортування відбувається всередині груп (закріплені/звичайні).
     * @param type Тип сортування (за датою або за назвою).
     */
    void sortNotes(SortType type);

    // --- Методи для збереження, завантаження, експорту та імпорту ---

    /**
     * @brief Зберігає всі дані (схеми, нотатки, статистику) у файл JSON.
     * @param filePath Шлях до файлу для збереження.
     */
    void saveToFile(const QString& filePath) const;

    /**
     * @brief Завантажує всі дані з файлу JSON.
     * @param filePath Шлях до файлу для завантаження.
     */
    void loadFromFile(const QString& filePath);

    /**
     * @brief Експортує одну конкретну нотатку у файл JSON.
     * @param index Індекс нотатки, яку потрібно експортувати.
     * @param filePath Шлях до файлу для експорту.
     */
    void exportNote(int index, const QString& filePath) const;

    /**
     * @brief Експортує нотатку у файл PDF з форматуванням.
     *
     * Створює документ, що містить заголовок, дату, теги, зображення та поля нотатки.
     * @param index Індекс нотатки.
     * @param filePath Шлях для збереження PDF файлу.
     */
    void exportNoteToPdf(int index, const QString& filePath) const;

    /**
     * @brief Імпортує нотатку з файлу JSON і додає її до сховища.
     * @param filePath Шлях до файлу для імпорту.
     */
    void importNote(const QString& filePath);

    // --- Методи для статистики ---

    /**
     * @brief Додає час сесії до статистики використання.
     *
     * Зберігає час у хвилинах/секундах для поточної дати.
     * @param seconds Тривалість сесії у секундах.
     */
    void addUsageTime(int seconds);

    /**
     * @brief Повертає історію використання програми.
     * @return Мапа, де ключ - дата (рядок), значення - час використання (секунди).
     */
    QMap<QString, int> getUsageStats() const;

 /**
 * @brief Запускає бенчмаркінг функцій saveToFile та loadFromFile.
 *
 * Генерує N тестових нотаток і вимірює час серіалізації/десеріалізації.
 * @param noteCount Кількість нотаток для тесту.
 * @return QPair<qint64, qint64> (Час збереження, Час завантаження) у мс.
 */
 QPair<qint64, qint64> runSystemBenchmark(int noteCount);

private:
 QList<Note> generateTestNotes(int count) const;  ///< Приватний метод для генерації тестових нотаток
    QList<Schema> m_schemas;   ///< Внутрішнє сховище для списку всіх схем.
    QList<Note> m_notes;       ///< Внутрішнє сховище для списку всіх нотаток.
    QMap<QString, int> m_usageStats; ///< Зберігає статистику часу використання (Дата -> Секунди).
};
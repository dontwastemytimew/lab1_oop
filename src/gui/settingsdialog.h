#pragma once
#include <QDialog>
#include <QEvent>
#include "datamanager.h"
#include <QLineEdit>
#include "note.h"

namespace Ui { class SettingsDialog; }

/**
 * @class SettingsDialog
 * @brief Діалогове вікно для налаштування параметрів програми.
 *
 * @details Централізований інтерфейс, який дозволяє користувачеві керувати
 * зовнішнім виглядом (теми), мовою, а також запускати службові модулі
 * (бенчмаркінг, статистика). Приймає вказівник на DataManager для доступу
 * до статистики нотаток.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор вікна налаштувань.
     *
     * @details Отримує вказівник на DataManager для доступу до даних нотаток,
     * необхідних для побудови графіків статистики.
     * @param dataManager Вказівник на центральне сховище даних.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit SettingsDialog(DataManager* dataManager, QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    /**
     * @brief Слот для запуску порівняльного тесту продуктивності.
     *
     * @details Генерує великий масив тестових даних, заміряє час серіалізації
     * за допомогою RapidJSON та nlohmann/json і викликає @ref plotResults
     * для візуалізації результатів.
     */
    void on_runBenchmarkButton_clicked();

    /**
     * @brief Слот, що викликається при зміні обраної теми.
     *
     * @details Встановлює @c QPalette та @c QStyleSheet (QSS) для всієї програми,
     * перемикаючи її між світлою та темною темами.
     * @param index Індекс обраного елемента (0 - Світла, 1 - Темна).
     */
    void on_themeComboBox_currentIndexChanged(int index);

    /**
     * @brief Слот, що викликається при зміні обраної мови.
     *
     * @details Завантажує та встановлює новий об'єкт @c QTranslator
     * для програми. Викликає @c changeEvent для оновлення інтерфейсу.
     * @param index Індекс обраного елемента (0 - Українська, 1 - English).
     */
    void on_languageComboBox_currentIndexChanged(int index);

protected:
    /**
     * @brief Обробник подій для динамічного оновлення інтерфейсу.
     *
     * @details Перехоплює подію зміни мови (@c QEvent::LanguageChange), блокує сигнали
     * для запобігання рекурсії, оновлює всі тексти, відновлює індекси ComboBox
     * та перемальовує графіки.
     * @param event Подія, що надійшла від системи.
     */
    void changeEvent(QEvent *event) override;


private:
    Ui::SettingsDialog *ui; ///< Вказівник на об'єкти інтерфейсу, згенеровані @c uic.

    /**
     * @brief Малює стовпчикову діаграму з результатами тесту продуктивності.
     * @param nlohmannTime Час серіалізації (мс) для nlohmann/json.
     * @param rapidJsonTime Час серіалізації (мс) для RapidJSON.
     */
    void plotResults(double nlohmannTime, double rapidJsonTime);

    /**
     * @brief Завантажує та застосовує QSS-файл для обраної теми.
     * @param enable Якщо true, застосовує темну тему; якщо false — світлу.
     */
    void setDarkTheme(bool enable);

    DataManager* m_dataManager; ///< Вказівник на сховище даних.

    /**
     * @brief Малює стовпчикову діаграму використання програми по днях.
     *
     * @details Використовує @c QCustomPlot для візуалізації @c m_usageStats.
     */
    void setupStatisticsChart();
};
#pragma once
#include <QDialog>
#include "datamanager.h"
#include <QEvent>

namespace Ui { class StatisticsDialog; }

/**
 * @class StatisticsDialog
 * @brief Діалогове вікно для відображення статистики використання програми.
 *
 * @details Клас керує модулем аналітики. Він отримує історичні дані використання
 * (час сесій) та структуру нотаток із @c DataManager і візуалізує їх
 * за допомогою зовнішньої бібліотеки @c QCustomPlot. Є частиною
 * функціоналу, що демонструє "значне доопрацювання" для Лабораторної 3.
 */
class StatisticsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор діалогового вікна статистики.
     *
     * @details При ініціалізації вікно отримує вказівник на центральне сховище даних
     * для доступу до історії активності користувача.
     * @param dataManager Вказівник на центральне сховище даних.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit StatisticsDialog(DataManager* dataManager, QWidget *parent = nullptr);
    ~StatisticsDialog();

private:
    /**
     * @brief Налаштовує та малює графік статистики.
     *
     * @details Цей метод збирає статистику використання по днях (@c m_usageStats),
     * конвертує час у хвилини, форматує вісь X (дати) і будує стовпчикову діаграму
     * (@c QCPBars) для візуалізації активності.
     */
    void setupChart();

    Ui::StatisticsDialog *ui;      ///< Вказівник на об'єкти інтерфейсу, згенеровані @c uic.
    DataManager* m_dataManager;    ///< Вказівник на сховище даних.
};
#pragma once
#include <QDialog>
#include <QEvent>
#include "note_repository.h"

namespace Ui { class StatisticsDialog; }

/**
 * @class StatisticsDialog
 * @brief Діалогове вікно для відображення статистики використання програми.
 *
 * @details Клас керує модулем аналітики. Він отримує історичні дані використання
 * (час сесій) та структуру нотаток із @c NoteRepository і візуалізує їх
 * за допомогою зовнішньої бібліотеки @c QCustomPlot.
 */
class StatisticsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор діалогового вікна статистики.
     *
     * @details При ініціалізації вікно отримує вказівник на репозиторій
     * для доступу до даних.
     * @param repository Вказівник на репозиторій нотаток.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit StatisticsDialog(NoteRepository* repository, QWidget *parent = nullptr);
    ~StatisticsDialog();

private:
    /**
     * @brief Налаштовує та малює графік статистики.
     */
    void setupChart();

    Ui::StatisticsDialog *ui;      ///< Вказівник на об'єкти інтерфейсу.
    NoteRepository* m_repository;  ///< Вказівник на репозиторій.
};
#pragma once
#include <QMainWindow>
#include "datamanager.h"
#include <QListWidgetItem>
#include <QElapsedTimer>
#include <QCloseEvent>
#include <QPoint>

namespace Ui { class MainWindow; }

/**
 * @class MainWindow
 * @brief Головне вікно програми та центр управління.
 *
 * Відповідає за відображення основного інтерфейсу, списку нотаток та кнопок
 * для основних дій. Виступає як центральний вузол, що координує роботу
 * інших дочірніх вікон та DataManager.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Конструктор головного вікна.
     * @param parent Вказівник на батьківський віджет (зазвичай nullptr).
     */
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief Оновлює список нотаток на екрані.
     *
     * @details Викликає сортування даних у DataManager, очищує QListWidget,
     * та заповнює його новими кастомними віджетами нотаток (`NoteWidget`),
     * враховуючи поточний текст у полі пошуку.
     */
    void updateNotesList();

private slots:
    // --- Слоти для кнопок ---
    void on_manageSchemasButton_clicked();
    void on_createNoteButton_clicked();

    /**
     * @brief Інкапсулює логіку видалення нотатки.
     *
     * Визначає обраний елемент, викликає видалення з DataManager та оновлює список.
     * Викликається з контекстного меню та кнопки "Видалити".
     */
    void deleteNoteLogic();

    void on_searchButton_clicked();
    void on_importNoteButton_clicked();
    void on_settingsButton_clicked();

    /**
     * @brief Слот, що спрацьовує при зміні типу сортування у ComboBox.
     *
     * Запускає процес сортування нотаток у сховищі DataManager згідно з обраним типом,
     * а потім оновлює відображення списку.
     * @param index Індекс обраного елемента, що відповідає типу сортування (SortType).
     */
    void on_sortComboBox_currentIndexChanged(int index);

    // --- Слоти для взаємодії зі списком нотаток ---
    /**
     * @brief Викликається при подвійному кліку на елемент списку нотаток.
     *
     * Відкриває вікно NoteEditor для редагування обраної нотатки.
     * @param item Елемент, на який було здійснено клік.
     */
    void onNoteDoubleClicked(QListWidgetItem *item);

    /**
     * @brief Викликається при кліку правою кнопкою миші на списку нотаток.
     *
     * Генерує та відображає контекстне меню (Закріпити/Відкріпити, Експорт, Видалити).
     * @param pos Позиція курсора.
     */
    void onNotesListContextMenuRequested(const QPoint &pos);


protected:
    /**
     * @brief Перехоплює подію закриття вікна для збереження даних.
     *
     * @details Перед закриттям викликає логіку для збереження всіх даних (схеми, нотатки)
     * у файл JSON, а також реєструє час, проведений у поточній сесії.
     * @param event Подія закриття.
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * @brief Обробник подій для динамічної зміни стану.
     *
     * @details Перехоплює подію зміни мови (`QEvent::LanguageChange`) для негайного
     * оновлення всіх перекладних рядків на головному вікні (Main Window) без
     * необхідності перезапуску програми.
     * @param event Подія, що надійшла від системи.
     */
    void changeEvent(QEvent *event) override;


private:
    Ui::MainWindow *ui; ///< Вказівник на об'єкти, створені у Qt Designer.
    DataManager m_dataManager; ///< Екземпляр "мозку" програми, що зберігає всі дані.

    /**
     * @brief Таймер для відстеження часу, проведеного в поточній сесії.
     *
     * @details Використовується для модуля статистики: час, що минув,
     * реєструється у DataManager при закритті програми.
     */
    QElapsedTimer m_sessionTimer;
};
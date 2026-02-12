#pragma once
#include <QDialog>
#include "datamanager.h"
#include <QEvent>

namespace Ui { class SchemaManager; }

/**
 * @class SchemaManager
 * @brief Діалогове вікно для керування схемами нотаток.
 *
 * @details Клас надає користувачеві інтерфейс для перегляду списку існуючих схем,
 * а також для їх створення, редагування та видалення. Є ключовою частиною
 * функціоналу, що дозволяє створювати гнучкі шаблони нотаток.
 */
class SchemaManager : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор вікна керування схемами.
     *
     * @details Ініціалізує діалог та отримує вказівник на центральне сховище даних
     * (@c DataManager) для керування схемами.
     * @param dataManager Вказівник на центральне сховище даних.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit SchemaManager(DataManager *dataManager, QWidget *parent = nullptr);
    ~SchemaManager();

    private slots:
    /**
     * @brief Слот, що викликається при натисканні на кнопку "Створити".
     *
     * @details Ініціалізує новий об'єкт @c SchemaEditor та запускає його в модальному
     * режимі для створення нової схеми. Після успішного завершення оновлює список схем.
     */
    void openSchemaEditor();

    /**
     * @brief Слот, що викликається при натисканні на кнопку "Видалити".
     *
     * @details Визначає обраний рядок у списку схем, викликає метод видалення
     * у @c DataManager та оновлює відображення.
     */
    void on_deleteSchemaButton_clicked();

    /**
     * @brief Слот, що викликається при натисканні на кнопку "Редагувати".
     *
     * @details Відкриває @c SchemaEditor, передаючи йому існуючу схему. Якщо редагування
     * завершується успіхом, викликає метод оновлення @c DataManager.
     */
    void on_editSchemaButton_clicked();

    /**
     * @brief Слот, що викликається при натисканні на кнопку "Закрити".
     *
     * @details Викликає метод @c QDialog::close(), закриваючи поточне діалогове вікно.
     */
    void on_closeButton_clicked();

protected:
    /**
     * @brief Обробник подій для динамічної зміни стану.
     *
     * @details Перехоплює подію зміни мови (@c QEvent::LanguageChange) для
     * негайного оновлення всіх перекладних рядків на вікні діалогу.
     * @param event Подія, що надійшла від системи.
     */
    void changeEvent(QEvent *event) override;

private:
    /**
     * @brief Оновлює список схем на екрані.
     *
     * @details Очищує віджет списку (@c schemasListWidget) та заповнює його
     * актуальними назвами схем, отриманими з @c DataManager.
     */
    void updateSchemasList();

    Ui::SchemaManager *ui;      ///< Вказівник на об'єкти інтерфейсу, згенеровані @c uic.
    DataManager *m_dataManager; ///< Вказівник на центральне сховище даних.
};
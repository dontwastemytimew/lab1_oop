#pragma once
#include <QDialog>
#include <QEvent>
#include "note_repository.h"

namespace Ui { class SchemaManager; }

/**
 * @class SchemaManager
 * @brief Діалогове вікно для керування схемами нотаток.
 *
 * @details Клас надає користувачеві інтерфейс для перегляду списку існуючих схем,
 * а також для їх створення, редагування та видалення.
 */
class SchemaManager : public QDialog {
 Q_OBJECT

public:
 /**
  * @brief Конструктор вікна керування схемами.
  *
  * @details Ініціалізує діалог та отримує вказівник на репозиторій
  * для керування схемами.
  * @param repository Вказівник на репозиторій нотаток (NoteRepository).
  * @param parent Вказівник на батьківський віджет.
  */
 explicit SchemaManager(NoteRepository *repository, QWidget *parent = nullptr);
 ~SchemaManager();

 private slots:
 /**
  * @brief Слот, що викликається при натисканні на кнопку "Створити".
  */
 void openSchemaEditor();

 /**
  * @brief Слот, що викликається при натисканні на кнопку "Видалити".
  */
 void on_deleteSchemaButton_clicked();

 /**
  * @brief Слот, що викликається при натисканні на кнопку "Редагувати".
  */
 void on_editSchemaButton_clicked();

 /**
  * @brief Слот, що викликається при натисканні на кнопку "Закрити".
  */
 void on_closeButton_clicked();

protected:
 /**
  * @brief Обробник подій для динамічної зміни стану.
  */
 void changeEvent(QEvent *event) override;

private:
 /**
  * @brief Оновлює список схем на екрані.
  */
 void updateSchemasList();

 Ui::SchemaManager *ui;      ///< Вказівник на об'єкти інтерфейсу.
 NoteRepository *m_repository; ///< Вказівник на репозиторій.
};
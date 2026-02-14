#pragma once
#include <QDialog>
#include "note_repository.h"
#include <QLineEdit>
#include "note.h"


namespace Ui { class NoteEditor; }

/**
 * @class NoteEditor
 * @brief Діалогове вікно для створення та редагування однієї нотатки.
 *
 * @details "Розумний" редактор, який динамічно генерує поля для вводу на основі
 * обраної користувачем схеми. Клас також відповідає за валідацію,
 * завантаження та попередній перегляд прикріплених зображень.
 */
class NoteEditor : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор для створення нової нотатки.
     * @param repository Вказівник на репозиторій нотаток.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit NoteEditor(NoteRepository *repository, QWidget *parent = nullptr);

    /**
     * @brief Конструктор для редагування існуючої нотатки.
     *
     * @details Завантажує всі дані, включаючи поля, теги та зображення,
     * з об'єкта @c noteToEdit в інтерфейс.
     * @param repository Вказівник на репозиторій нотаток.
     * @param noteToEdit Нотатка, дані якої потрібно завантажити в редактор.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit NoteEditor(NoteRepository *repository, const Note& noteToEdit, QWidget *parent = nullptr);

    ~NoteEditor();

    /**
     * @brief Збирає дані з усіх полів вводу та повертає готовий об'єкт Note.
     *
     * @details Цей метод використовується для збереження як нових, так і відредагованих
     * нотаток. Він також гарантує збереження існуючих тегів та зображень.
     * @return Створений або оновлений об'єкт Note.
     */
    Note getNote() const;

private slots:
    /**
     * @brief Слот, що викликається при зміні обраної схеми у випадаючому списку.
     *
     * @details Динамічно генерує форму для вводу даних (поле + мітка) відповідно
     * до полів обраної схеми.
     * @param index Індекс обраної схеми.
     */
    void onSchemaSelected(int index);

    /**
     * @brief Перевизначений слот для валідації даних перед закриттям вікна.
     *
     * @details Перевіряє, що назва нотатки не є порожньою, і виводить
     * @c QMessageBox у разі помилки.
     */
    void accept() override;

    /**
     * @brief Слот для завантаження зображення користувачем.
     * * @details Відкриває діалог вибору файлу, зчитує зображення та кодує його
     * у формат Base64 для подальшої обробки та збереження.
     */
    void on_addImageButton_clicked();

protected:
    /**
     * @brief Обробник подій для динамічного оновлення інтерфейсу.
     * * @details Перехоплює подію зміни мови (@c QEvent::LanguageChange)
     * для негайного оновлення всіх елементів інтерфейсу, що підтримують інтернаціоналізацію.
     * @param event Подія, що надійшла від системи.
     */
    void changeEvent(QEvent *event) override;

private:
    /**
     * @brief Декодує рядок Base64 та відображає зображення у вікні редактора.
     * @param base64 Рядок із закодованими даними зображення.
     */
    void displayImage(const QString& base64);

    Ui::NoteEditor *ui;             ///< Вказівник на об'єкти інтерфейсу.
    NoteRepository *m_repository;   ///< Вказівник на репозиторій.
    QList<QLineEdit*> m_fieldInputs;///< Тимчасове сховище для вказівників на динамічно створені поля вводу.

    /**
     * @brief Зберігає теги оригінальної нотатки під час редагування.
     *
     * @details Це необхідно для того, щоб запобігти видаленню тегів,
     * оскільки вони не відображаються у вікні редагування нотатки.
     */
    QSet<QString> m_originalTags;

    /**
     * @brief Зберігає закодоване Base64 зображення для передачі в об'єкт Note.
     * * @details Цей рядок є буфером, який містить поточне зображення.
     */
    QString m_currentImageBase64;
};
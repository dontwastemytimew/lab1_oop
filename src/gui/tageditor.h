#pragma once
#include <QDialog>
#include <QSet>
#include <QEvent>

namespace Ui { class TagEditor; }

/**
 * @class TagEditor
 * @brief Діалогове вікно для керування тегами однієї нотатки.
 *
 * @details Клас надає користувачеві інтерфейс для перегляду списку існуючих тегів,
 * їх видалення та додавання нових. Усі зміни зберігаються в тимчасовому наборі
 * до натискання кнопки "OK".
 */
class TagEditor : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор вікна редактора тегів.
     *
     * @details Ініціалізує діалог, завантажуючи існуючі теги у внутрішнє сховище.
     * @param currentTags Набір існуючих тегів, які потрібно відобразити.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit TagEditor(const QSet<QString>& currentTags, QWidget *parent = nullptr);
    /**
     * @brief Деструктор класу.
     */
    ~TagEditor();

    /**
     * @brief Повертає оновлений набір тегів після редагування.
     *
     * @details Ця функція викликається батьківським вікном для отримання фінального
     * списку тегів, який буде збережений у @c Note.
     * @return QSet<QString> з фінальним набором тегів.
     */
    QSet<QString> getTags() const;

    private slots:
    /**
     * @brief Слот, що викликається при натисканні на кнопку "Додати тег".
     *
     * @details Зчитує текст з поля вводу. Виконує валідацію на порожній рядок
     * та на дублювання, після чого додає тег до тимчасового сховища @c m_tags
     * та оновлює список на екрані.
     */
    void on_addTagButton_clicked();

protected:
    /**
     * @brief Обробник подій для динамічного оновлення інтерфейсу.
     *
     * @details При отриманні події зміни мови (@c QEvent::LanguageChange)
     * оновлює всі елементи вікна, що підтримують інтернаціоналізацію.
     * @param event Подія, що надійшла від системи.
     */
    void changeEvent(QEvent *event) override;

private:
    Ui::TagEditor *ui;      ///< Вказівник на об'єкти інтерфейсу, згенеровані @c uic.

    /**
     * @brief Тимчасове сховище для набору тегів.
     *
     * @details Це @c QSet, що гарантує унікальність тегів. Він є внутрішнім
     * буфером, який модифікується користувачем і повертається через @c getTags().
     */
    QSet<QString> m_tags;
};
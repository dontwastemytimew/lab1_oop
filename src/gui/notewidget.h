#pragma once
#include <QWidget>
#include "note.h"

namespace Ui { class NoteWidget; }

/**
 * @class NoteWidget
 * @brief Кастомний віджет для відображення однієї нотатки у списку.
 *
 * Цей клас відповідає за візуальне представлення однієї нотатки,
 * показуючи її назву, дату створення та теги у вигляді красивої плашки.
 */
class NoteWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор віджета нотатки.
     * @param note Об'єкт нотатки, дані якої потрібно відобразити.
     * @param parent Вказівник на батьківський віджет.
     */
    explicit NoteWidget(const Note& note, QWidget *parent = nullptr);
    ~NoteWidget();

private:
    Ui::NoteWidget *ui; ///< Вказівник на об'єкти інтерфейсу (QLabel'и).
};
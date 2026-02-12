#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "schemamanager.h"
#include "noteeditor.h"
#include <QDebug>
#include <QMenu>
#include <QListWidgetItem>
#include "notewidget.h"
#include "tageditor.h"
#include <QFileDialog>
#include "settingsdialog.h"
#include "statisticsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->sortComboBox->clear();
    ui->sortComboBox->addItem(tr("Спочатку нові"));   // index 0
    ui->sortComboBox->addItem(tr("Спочатку старі"));  // index 1
    ui->sortComboBox->addItem(tr("Назва (А-Я)"));     // index 2
    ui->sortComboBox->addItem(tr("Назва (Я-А)"));     // index 3

    m_sessionTimer.start();

    setWindowTitle(tr("Редактор структурованих нотаток"));

    ui->notesListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->notesListWidget, &QWidget::customContextMenuRequested, this, &MainWindow::onNotesListContextMenuRequested);
    connect(ui->notesListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onNoteDoubleClicked);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::updateNotesList);
    m_dataManager.loadFromFile("data.json");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_manageSchemasButton_clicked()
{
    qInfo() << tr("Відкрито вікно 'Керування схемами'.");
    SchemaManager schemaManagerWindow(&m_dataManager, this);
    schemaManagerWindow.exec();
    qInfo() << tr("Вікно 'Керування схемами' закрито.");
}

void MainWindow::updateNotesList()
{
    SortType type = SortType::ByDateNewest;
    int index = ui->sortComboBox->currentIndex();

    if (index == 1) type = SortType::ByDateOldest;
    else if (index == 2) type = SortType::ByNameAZ;
    else if (index == 3) type = SortType::ByNameZA;

    m_dataManager.sortNotes(type);

    QString searchText = ui->searchLineEdit->text();
    ui->notesListWidget->clear();

    for (const auto& note : m_dataManager.getNotes()) {
        if (searchText.isEmpty() || note.getTitle().contains(searchText, Qt::CaseInsensitive))
        {
            auto item = new QListWidgetItem();
            auto noteWidget = new NoteWidget(note);
            item->setSizeHint(noteWidget->sizeHint());
            ui->notesListWidget->addItem(item);
            ui->notesListWidget->setItemWidget(item, noteWidget);
        }
    }
}

void MainWindow::on_createNoteButton_clicked() {
    NoteEditor editor(&m_dataManager, this);

    if (editor.exec() == QDialog::Accepted)
    {
        Note newNote = editor.getNote();
        qInfo() << tr("Створено нову нотатку з назвою: %1").arg(newNote.getTitle());
        m_dataManager.addNote(newNote);
        updateNotesList();
    } else {
        qInfo() << tr("Створення нової нотатки було скасовано користувачем.");
    }
}


void MainWindow::deleteNoteLogic()
{
    int currentIndex = ui->notesListWidget->currentRow();
    if (currentIndex < 0) {
        qWarning() << tr("Спроба видалити нотатку, коли нічого не вибрано.");
        return;
    }
    qInfo() << tr("Видалено нотатку з індексом: %1").arg(currentIndex);
    m_dataManager.removeNote(currentIndex);
    updateNotesList();
}

void MainWindow::onNotesListContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* item = ui->notesListWidget->itemAt(pos);
    if (!item) return;

    int currentIndex = ui->notesListWidget->row(item);
    QMenu contextMenu(this);

    QAction *pinAction = contextMenu.addAction(tr("Закріпити/Відкріпити"));
    contextMenu.addSeparator();

    QAction *editTagsAction = contextMenu.addAction(tr("Редагувати теги"));
    contextMenu.addSeparator();
    QAction *exportAction = contextMenu.addAction(tr("Експортувати (.json)"));
    QAction *exportPdfAction = contextMenu.addAction(tr("Експортувати (PDF)"));
    contextMenu.addSeparator();
    QAction *deleteAction = contextMenu.addAction(tr("Видалити нотатку"));

    QAction *selectedAction = contextMenu.exec(ui->notesListWidget->mapToGlobal(pos));

    if (selectedAction == pinAction) {
        Note& note = m_dataManager.getNotes()[currentIndex];
        note.setPinned(!note.isPinned());
        updateNotesList();
    }

    if (selectedAction == deleteAction) {
        qDebug() << tr("Користувач обрав 'Видалити нотатку' з контекстного меню.");
        deleteNoteLogic();
    } else if (selectedAction == editTagsAction) {
        qDebug() << tr("Користувач обрав 'Редагувати теги' з контекстного меню.");
        Note& noteToEdit = m_dataManager.getNotes()[currentIndex];
        TagEditor editor(noteToEdit.getTags(), this);
        if (editor.exec() == QDialog::Accepted) {
            noteToEdit.setTags(editor.getTags());
            m_dataManager.updateNote(currentIndex, noteToEdit);
            updateNotesList();
        }
    } else if (selectedAction == exportAction) {
        qDebug() << tr("Користувач обрав 'Експортувати' з контекстного меню.");
        QString defaultFileName = m_dataManager.getNotes()[currentIndex].getTitle();
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Експортувати нотатку"),
            defaultFileName + ".json",
            tr("JSON Files (*.json)"));

        if (!fileName.isEmpty()) {
            m_dataManager.exportNote(currentIndex, fileName);
        }
    }
    else if (selectedAction == exportPdfAction) {
        QString defaultFileName = m_dataManager.getNotes()[currentIndex].getTitle();

        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Зберегти як PDF"),
            defaultFileName + ".pdf",
            tr("PDF Files (*.pdf)"));

        if (!fileName.isEmpty()) {
            m_dataManager.exportNoteToPdf(currentIndex, fileName);
        }
    }
}

void MainWindow::onNoteDoubleClicked(QListWidgetItem *item)
{
    int currentIndex = ui->notesListWidget->row(item);
    if (currentIndex < 0) return;

    const Note& noteToEdit = m_dataManager.getNotes()[currentIndex];
    qInfo() << tr("Відкрито редактор для нотатки: %1").arg(noteToEdit.getTitle());

    NoteEditor editor(&m_dataManager, noteToEdit, this);

    if (editor.exec() == QDialog::Accepted)
    {
        qInfo() << tr("Зміни до нотатки '%1' збережено.").arg(noteToEdit.getTitle());
        Note updatedNote = editor.getNote();
        m_dataManager.updateNote(currentIndex, updatedNote);
        updateNotesList();
    }
}

void MainWindow::on_searchButton_clicked()
{
    QString searchText = ui->searchLineEdit->text();
    if (!searchText.isEmpty()) {
        qInfo() << tr("Виконано пошук за запитом: %1").arg(searchText);
    } else {
        qInfo() << tr("Пошук скинуто.");
    }
    updateNotesList();
}

void MainWindow::on_importNoteButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Імпортувати нотатку"),
        QDir::homePath(),
        tr("JSON Files (*.json)"));

    if (!fileName.isEmpty()) {
        m_dataManager.importNote(fileName);
        updateNotesList();
    }
}

void MainWindow::on_settingsButton_clicked() {
    SettingsDialog dialog(&m_dataManager, this);
    dialog.exec();
}


void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);

        setWindowTitle(tr("Редактор структурованих нотаток"));
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    qint64 sessionSeconds = m_sessionTimer.elapsed() / 1000;
    m_dataManager.addUsageTime(sessionSeconds);

    qInfo() << "Сесія тривала" << sessionSeconds << "секунд.";

    m_dataManager.saveToFile("data.json");
    QMainWindow::closeEvent(event);
}

void MainWindow::on_sortComboBox_currentIndexChanged(int index)
{
    updateNotesList();
}
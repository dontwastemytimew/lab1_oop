#include "noteeditor.h"
#include "ui_noteeditor.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include <QPixmap>
#include <QByteArray>

NoteEditor::NoteEditor(DataManager *dataManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoteEditor),
    m_dataManager(dataManager)
{
    ui->setupUi(this);
    setWindowTitle(tr("Редактор нотатки"));

    for (const auto& schema : m_dataManager->getSchemas()) {
        ui->schemaComboBox->addItem(schema.getName());
    }


    connect(ui->schemaComboBox, &QComboBox::currentIndexChanged, this, &NoteEditor::onSchemaSelected);
    onSchemaSelected(0);
}

NoteEditor::~NoteEditor() {
    delete ui;
}

void NoteEditor::onSchemaSelected(int index)
{
    const auto& schemas = m_dataManager->getSchemas();
    if (index < 0 || index >= schemas.size()) return;
    const Schema& selectedSchema = schemas[index];

    qInfo() << tr("У редакторі нотаток обрано схему '%1'. Генерується %2 полів.").arg(selectedSchema.getName()).arg(selectedSchema.getFields().size());

    if (ui->fieldsScrollArea->widget()) {
        delete ui->fieldsScrollArea->widget();
    }
    m_fieldInputs.clear();

    QWidget *fieldsContainer = new QWidget;
    QFormLayout *formLayout = new QFormLayout(fieldsContainer);

    for (const auto& field : selectedSchema.getFields()) {
        QLabel *label = new QLabel(field.name);
        QLineEdit *lineEdit = new QLineEdit();
        formLayout->addRow(label, lineEdit);
        m_fieldInputs.append(lineEdit);
    }

    ui->fieldsScrollArea->setWidget(fieldsContainer);
}

Note NoteEditor::getNote() const
{
    QString title = ui->titleLineEdit->text();
    int schemaIndex = ui->schemaComboBox->currentIndex();
    const Schema& selectedSchema = m_dataManager->getSchemas()[schemaIndex];

    Note newNote(title, schemaIndex);

    for (int i = 0; i < selectedSchema.getFields().size(); ++i) {
        QString fieldName = selectedSchema.getFields()[i].name;
        QString fieldValue = m_fieldInputs[i]->text();
        newNote.addField(fieldName, fieldValue);
    }

    newNote.setTags(m_originalTags);

    newNote.setImage(m_currentImageBase64);

    return newNote;
}

NoteEditor::NoteEditor(DataManager *dataManager, const Note &noteToEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoteEditor),
    m_dataManager(dataManager)
{
    ui->setupUi(this);
    setWindowTitle(tr("Редактор нотатки"));

    for (const auto& schema : m_dataManager->getSchemas()) {
        ui->schemaComboBox->addItem(schema.getName());
    }

    ui->titleLineEdit->setText(noteToEdit.getTitle());
    ui->schemaComboBox->setCurrentIndex(noteToEdit.getSchemaId());
    ui->schemaComboBox->setEnabled(false);

    onSchemaSelected(noteToEdit.getSchemaId());
    const auto& fields = noteToEdit.getFields();
    const auto& schemaFields = m_dataManager->getSchemas()[noteToEdit.getSchemaId()].getFields();

    for (int i = 0; i < schemaFields.size(); ++i) {
        const QString& fieldName = schemaFields[i].name;
        if (fields.contains(fieldName) && i < m_fieldInputs.size()) {
            m_fieldInputs[i]->setText(fields.value(fieldName));
        }
    }


    m_currentImageBase64 = noteToEdit.getImage();
    if (!m_currentImageBase64.isEmpty()) {
        displayImage(m_currentImageBase64);
        ui->addImageButton->setText(tr("Змінити фото"));
    }

    m_originalTags = noteToEdit.getTags();
}

void NoteEditor::accept()
{
    if (ui->titleLineEdit->text().trimmed().isEmpty()) {
        qWarning() << tr("Помилка валідації: Назва нотатки не може бути порожньою.");
        QMessageBox::warning(this, tr("Помилка валідації"), tr("Назва нотатки не може бути порожньою!"));
    } else {
        QDialog::accept();
    }
}

void NoteEditor::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        setWindowTitle(tr("Редактор нотатки"));
    }
    QDialog::changeEvent(event);
}

void NoteEditor::on_addImageButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Виберіть зображення"), "",
        tr("Зображення (*.png *.jpg *.jpeg *.bmp)"));

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray imageData = file.readAll();

        m_currentImageBase64 = QString::fromLatin1(imageData.toBase64());

        displayImage(m_currentImageBase64);
        ui->addImageButton->setText(tr("Змінити фото"));

        qInfo() << "Додано зображення до нотатки (розмір:" << imageData.size() << "байт)";
    }
}

void NoteEditor::displayImage(const QString& base64)
{
    if (base64.isEmpty()) return;

    QByteArray imageData = QByteArray::fromBase64(base64.toLatin1());
    QPixmap pixmap;
    if (pixmap.loadFromData(imageData)) {
        ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
    } else {
        ui->imageLabel->setText(tr("Не вдалося показати фото"));
    }
}
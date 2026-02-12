#include "schemaeditor.h"
#include "ui_schemaeditor.h"
#include <QMessageBox>

SchemaEditor::SchemaEditor(QWidget *parent) :
    QDialog(parent), ui(new Ui::SchemaEditor)
{
    ui->setupUi(this);
    setWindowTitle(tr("Редактор схеми"));
}

SchemaEditor::~SchemaEditor() {
    delete ui;
}

Schema SchemaEditor::getSchema() const
{
    Schema newSchema(ui->schemaNameLineEdit->text());
    for (const auto& field : m_tempFields) {
        newSchema.addField(field);
    }
    return newSchema;
}

void SchemaEditor::on_addFieldButton_clicked() {
    QString fieldName = ui->fieldNameLineEdit->text().trimmed();

    if (!fieldName.isEmpty()) {
        Field newField;
        newField.name = fieldName;
        newField.type = tr("Текст");
        m_tempFields.append(newField);
        ui->fieldsListWidget->addItem(fieldName);
        ui->fieldNameLineEdit->clear();
        qInfo() << tr("До схеми додано нове поле: %1").arg(fieldName);
    } else {
        qWarning() << tr("Спроба додати поле без назви.");
    }
}

SchemaEditor::SchemaEditor(const Schema &schemaToEdit, QWidget *parent) :
    QDialog(parent), ui(new Ui::SchemaEditor)
{
    ui->setupUi(this);
    setWindowTitle(tr("Редактор схеми"));

    ui->schemaNameLineEdit->setText(schemaToEdit.getName());
    for (const auto& field : schemaToEdit.getFields()) {
        m_tempFields.append(field);
        ui->fieldsListWidget->addItem(field.name);
    }
}

void SchemaEditor::accept()
{
    if (ui->schemaNameLineEdit->text().trimmed().isEmpty()) {
        qWarning() << tr("Помилка валідації: Назва схеми не може бути порожньою.");
        QMessageBox::warning(this, tr("Помилка валідації"), tr("Назва схеми не може бути порожньою!"));
    } else {
        QDialog::accept();
    }
}

void SchemaEditor::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        setWindowTitle(tr("Редактор схеми"));
    }
    QDialog::changeEvent(event);
}
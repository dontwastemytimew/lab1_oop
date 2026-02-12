#include "schemamanager.h"
#include "ui_schemamanager.h"
#include "schemaeditor.h"
#include <QDebug>

SchemaManager::SchemaManager(DataManager *dataManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SchemaManager),
    m_dataManager(dataManager)
{
    ui->setupUi(this);
    setWindowTitle(tr("Керування схемами"));

    connect(ui->createSchemaButton, &QPushButton::clicked, this, &SchemaManager::openSchemaEditor);

    updateSchemasList();
}

SchemaManager::~SchemaManager() {
    delete ui;
}

void SchemaManager::updateSchemasList()
{
    ui->schemasListWidget->clear();
    for (const auto& schema : m_dataManager->getSchemas()) {
        ui->schemasListWidget->addItem(schema.getName());
    }
}

void SchemaManager::openSchemaEditor()
{
    SchemaEditor editor(this);
    if (editor.exec() == QDialog::Accepted)
    {
        Schema newSchema = editor.getSchema();
        qInfo() << tr("Створено нову схему з назвою: %1").arg(newSchema.getName());
        m_dataManager->addSchema(newSchema);
        updateSchemasList();
    } else {
        qInfo() << tr("Створення нової схеми було скасовано користувачем.");
    }
}

void SchemaManager::on_deleteSchemaButton_clicked()
{
    int currentIndex = ui->schemasListWidget->currentRow();
    if (currentIndex < 0) {
        qWarning() << tr("Спроба видалити схему, коли нічого не вибрано.");
        return;
    }

    qInfo() << tr("Видалено схему з індексом: %1").arg(currentIndex);
    m_dataManager->removeSchema(currentIndex);
    updateSchemasList();
}

void SchemaManager::on_editSchemaButton_clicked()
{
    int currentIndex = ui->schemasListWidget->currentRow();
    if (currentIndex < 0) {
        qWarning() << tr("Спроба редагувати схему, коли нічого не вибрано.");
        return;
    }

    const Schema& schemaToEdit = m_dataManager->getSchemas()[currentIndex];
    qInfo() << tr("Відкрито редактор для схеми: %1").arg(schemaToEdit.getName());

    SchemaEditor editor(schemaToEdit, this);
    if (editor.exec() == QDialog::Accepted)
    {
        qInfo() << tr("Зміни до схеми '%1' збережено.").arg(schemaToEdit.getName());
        Schema updatedSchema = editor.getSchema();
        m_dataManager->updateSchema(currentIndex, updatedSchema);
        updateSchemasList();
    }
}

void SchemaManager::on_closeButton_clicked()
{
    this->close();
}

void SchemaManager::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        setWindowTitle(tr("Керування схемами"));
    }
    QDialog::changeEvent(event);
}
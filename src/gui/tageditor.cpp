#include "tageditor.h"
#include "ui_tageditor.h"
#include <QDebug>

TagEditor::TagEditor(const QSet<QString>& currentTags, QWidget *parent) :
    QDialog(parent), ui(new Ui::TagEditor), m_tags(currentTags) {
    ui->setupUi(this);
    setWindowTitle(tr("Редактор тегів"));

    for (const auto& tag : m_tags) {
        ui->tagsListWidget->addItem(tag);
    }
}

TagEditor::~TagEditor() {
    delete ui;
}

QSet<QString> TagEditor::getTags() const {
    return m_tags;
}

void TagEditor::on_addTagButton_clicked() {
    QString newTag = ui->tagLineEdit->text().trimmed();
    if (!newTag.isEmpty() && !m_tags.contains(newTag)) {
        m_tags.insert(newTag);
        ui->tagsListWidget->addItem(newTag);
        ui->tagLineEdit->clear();
        qInfo() << tr("Додано новий тег: %1").arg(newTag);
    } else {
        qWarning() << tr("Спроба додати порожній або існуючий тег: %1").arg(newTag);
    }
}

void TagEditor::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        setWindowTitle(tr("Редактор тегів"));
    }
    QDialog::changeEvent(event);
}
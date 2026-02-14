#pragma once
#include <QDialog>
#include <QEvent>
#include "note_repository.h"
#include <QLineEdit>
#include "note.h"

namespace Ui { class SettingsDialog; }

/**
 * @class SettingsDialog
 * @brief Діалогове вікно для налаштування параметрів програми.
 */
class SettingsDialog : public QDialog {
 Q_OBJECT

public:
 /**
  * @brief Конструктор вікна налаштувань.
  * @param repository Вказівник на репозиторій нотаток.
  * @param parent Вказівник на батьківський віджет.
  */
 explicit SettingsDialog(NoteRepository* repository, QWidget *parent = nullptr);
 ~SettingsDialog();

 private slots:
     void on_runBenchmarkButton_clicked();
 void on_themeComboBox_currentIndexChanged(int index);
 void on_languageComboBox_currentIndexChanged(int index);

protected:
 void changeEvent(QEvent *event) override;

private:
 Ui::SettingsDialog *ui;
 void plotResults(double nlohmannTime, double rapidJsonTime);
 void setDarkTheme(bool enable);
 void setupStatisticsChart();

 /**
  * @brief Допоміжний метод для генерації тестових нотаток.
  * Використовується під час бенчмарку.
  */
 QList<Note> generateTestNotes(int count) const;

 NoteRepository* m_repository; ///< Вказівник на репозиторій.
};
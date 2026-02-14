#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "json.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "nanobench.h"
#include "qcustomplot.h"
#include <QElapsedTimer>
#include <vector>
#include <string>
#include <QTranslator>
#include <QLibraryInfo>
#include <QFile>
#include <QDebug>
#include <QRandomGenerator>
#include <QDate>

static QTranslator *appTranslator = nullptr;
static bool isDarkThemeActive = false;
static bool isEnglishActive = false;

using nlohmann_json = nlohmann::json;

SettingsDialog::SettingsDialog(NoteRepository* repository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    m_repository(repository)
{
    ui->setupUi(this);
    setWindowTitle(tr("Налаштування"));

    ui->tabWidget->setCurrentIndex(0);

    ui->languageComboBox->blockSignals(true);
    ui->languageComboBox->addItem(tr("Українська"), QVariant("uk_UA"));
    ui->languageComboBox->addItem(tr("English"), QVariant("en_US"));
    ui->languageComboBox->setCurrentIndex(isEnglishActive ? 1 : 0);
    ui->languageComboBox->blockSignals(false);

    ui->themeComboBox->blockSignals(true);
    ui->themeComboBox->addItem(tr("Світла"));
    ui->themeComboBox->addItem(tr("Темна"));
    ui->themeComboBox->setCurrentIndex(isDarkThemeActive ? 1 : 0);
    ui->themeComboBox->blockSignals(false);

    ui->plotWidget->xAxis->setLabel(tr("Бібліотека"));
    ui->plotWidget->yAxis->setLabel(tr("Час (мс)"));
    ui->plotWidget->legend->setVisible(true);
    ui->plotWidget->legend->setBrush(QBrush(QColor(255, 255, 255, 200)));
    ui->plotWidget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);

    connect(ui->themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::on_themeComboBox_currentIndexChanged);

    connect(ui->languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::on_languageComboBox_currentIndexChanged);

    setupStatisticsChart();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::on_runBenchmarkButton_clicked()
{
    ui->runBenchmarkButton->setEnabled(false);
    ui->runBenchmarkButton->setText(tr("Тестування..."));
    ui->resultLabel->setText(tr("Генерація даних..."));
    qApp->processEvents();

    const int LIBRARY_COUNT = 100000;
    std::vector<std::string> testData;
    testData.reserve(LIBRARY_COUNT);
    for(int i = 0; i < LIBRARY_COUNT; ++i) {
        testData.push_back("Test Note Content " + std::to_string(i));
    }

    ui->resultLabel->setText(tr("Тест nlohmann/json..."));
    qApp->processEvents();

    QElapsedTimer timer;
    timer.start();
    {
        nlohmann_json root;
        root["notes"] = nlohmann_json::array();
        for(const auto& str : testData) {
            nlohmann_json obj;
            obj["title"] = str;
            root["notes"].push_back(obj);
        }
        std::string res = root.dump();
    }
    double nlohmannTime = timer.elapsed();

    ui->resultLabel->setText(tr("Тест RapidJSON..."));
    qApp->processEvents();

    timer.restart();
    {
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("notes");
        writer.StartArray();
        for(const auto& str : testData) {
            writer.StartObject();
            writer.Key("title");
            writer.String(str.c_str());
            writer.EndObject();
        }
        writer.EndArray();
        writer.EndObject();
        std::string res = s.GetString();
    }
    double rapidJsonTime = timer.elapsed();

    plotResults(nlohmannTime, rapidJsonTime);

    QString winner = (rapidJsonTime < nlohmannTime) ? "RapidJSON" : "nlohmann";
    double diff = (nlohmannTime > rapidJsonTime) ? nlohmannTime/rapidJsonTime : rapidJsonTime/nlohmannTime;

    ui->resultLabel->setText(tr("Тест бібліотек (%1 елементів): Переможець: %2 (швидше в %3 разів!)")
                             .arg(LIBRARY_COUNT)
                             .arg(winner)
                             .arg(diff, 0, 'f', 2));

    ui->resultLabel->setText(tr("Системний тест I/O (JSON)..."));
    qApp->processEvents();

    const int SYSTEM_NOTE_COUNT = 5000;
    QString tempFilePath = "benchmark_temp_data.json";

    QList<Note> originalNotes = m_repository->getNotes();

    QList<Note> testNotes = generateTestNotes(SYSTEM_NOTE_COUNT);
    m_repository->getNotes() = testNotes;

    timer.restart();
    m_repository->save(tempFilePath);
    double systemSaveTime = timer.elapsed();

    m_repository->getNotes().clear();
    timer.restart();
    m_repository->load(tempFilePath);
    double systemLoadTime = timer.elapsed();

    QFile::remove(tempFilePath);
    m_repository->getNotes() = originalNotes;

    ui->resultLabel->setText(
        tr("Системний тест (%1 нотаток):\nЗбереження: %2 мс\nЗавантаження: %3 мс\nЗагалом: %4 мс")
        .arg(SYSTEM_NOTE_COUNT)
        .arg(systemSaveTime, 0, 'f', 1)
        .arg(systemLoadTime, 0, 'f', 1)
        .arg(systemSaveTime + systemLoadTime, 0, 'f', 1)
    );

    ui->runBenchmarkButton->setEnabled(true);
    ui->runBenchmarkButton->setText(tr("Запустити тест"));
}

QList<Note> SettingsDialog::generateTestNotes(int count) const {
    int schemaId = 0;
    if (m_repository->getSchemas().isEmpty()) {
        schemaId = 0;
    }

    QList<Note> testNotes;
    testNotes.reserve(count);

    for (int i = 0; i < count; ++i) {
        QString title = QString("Test Note %1").arg(i + 1);
        Note note(title, schemaId);

        note.addField("Benchmark Field", QString("Random Value %1").arg(QRandomGenerator::global()->generate()));

        if (i % 10 == 0) note.addTag("benchmark");
        if (i % 20 == 0) {
            note.setImage("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8BQDwAEhQGAhKmMIQAAAABJRU5ErkJggg==");
        }

        testNotes.append(note);
    }
    return testNotes;
}

void SettingsDialog::plotResults(double nlohmannTime, double rapidJsonTime)
{
    ui->plotWidget->clearPlottables();
    ui->plotWidget->clearItems();

    QCPBars *bars = new QCPBars(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    bars->setName(tr("Швидкість серіалізації"));
    bars->setWidth(0.6);

    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2;
    labels << "nlohmann/json" << "RapidJSON";

    QVector<double> keys, values;
    keys << 1 << 2;
    values << nlohmannTime << rapidJsonTime;
    bars->setData(keys, values);

    QColor loserColor("#d8bfd8");
    QColor winnerColor("#ff1493");

    QCPBars *winnerBar = new QCPBars(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    winnerBar->setWidth(0.6);
    winnerBar->setPen(Qt::NoPen);
    winnerBar->setBrush(winnerColor);

    if (rapidJsonTime < nlohmannTime) {
        winnerBar->addData(2, rapidJsonTime);
        bars->setBrush(loserColor);
    } else {
        winnerBar->addData(1, nlohmannTime);
        bars->setBrush(loserColor);
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->plotWidget->xAxis->setTicker(textTicker);
    ui->plotWidget->xAxis->setRange(0, 3);

    double maxVal = std::max(nlohmannTime, rapidJsonTime);
    ui->plotWidget->yAxis->setRange(0, maxVal * 1.3);
    ui->plotWidget->yAxis->setLabel(tr("Час виконання (мілісекунди)"));

    auto addLabel = [&](double x, double y, const QColor& color) {
        QCPItemText *text = new QCPItemText(ui->plotWidget);
        text->position->setCoords(x, y);
        text->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        text->setText(QString::number(y, 'f', 1) + " ms");
        text->setFont(QFont("Arial", 10, QFont::Bold));
        text->setColor(color);
    };

    addLabel(1, nlohmannTime, QColor("#804060"));
    addLabel(2, rapidJsonTime, winnerColor);

    ui->plotWidget->replot();
}

void SettingsDialog::on_languageComboBox_currentIndexChanged(int index)
{
    QString langCode = ui->languageComboBox->itemData(index).toString();
    isEnglishActive = (langCode == "en_US");

    if (appTranslator) {
        qApp->removeTranslator(appTranslator);
        delete appTranslator;
        appTranslator = nullptr;
    }

    if (isEnglishActive) {
        appTranslator = new QTranslator(qApp);
        if (appTranslator->load(":/i18n/lab3_en_US.qm")) {
            qApp->installTranslator(appTranslator);
        }
    }

    for (QWidget *widget : QApplication::allWidgets()) {
        QEvent event(QEvent::LanguageChange);
        QCoreApplication::sendEvent(widget, &event);
    }
}

void SettingsDialog::on_themeComboBox_currentIndexChanged(int index)
{
    bool newIsDark = (index == 1);
    if (newIsDark == isDarkThemeActive) return;
    isDarkThemeActive = newIsDark;
    setDarkTheme(newIsDark);
}

void SettingsDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->themeComboBox->blockSignals(true);
        ui->languageComboBox->blockSignals(true);

        ui->retranslateUi(this);
        setWindowTitle(tr("Налаштування"));

        ui->themeComboBox->clear();
        ui->themeComboBox->addItem(tr("Світла"));
        ui->themeComboBox->addItem(tr("Темна"));
        ui->themeComboBox->setCurrentIndex(isDarkThemeActive ? 1 : 0);

        ui->languageComboBox->clear();
        ui->languageComboBox->addItem(tr("Українська"), QVariant("uk_UA"));
        ui->languageComboBox->addItem(tr("English"), QVariant("en_US"));
        ui->languageComboBox->setCurrentIndex(isEnglishActive ? 1 : 0);

        ui->plotWidget->xAxis->setLabel(tr("Бібліотека"));
        ui->plotWidget->yAxis->setLabel(tr("Час (мс)"));
        ui->plotWidget->replot();

        setupStatisticsChart();

        ui->themeComboBox->blockSignals(false);
        ui->languageComboBox->blockSignals(false);
    }
    QDialog::changeEvent(event);
}

void SettingsDialog::setDarkTheme(bool enable)
{
    QString stylePath = enable ? ":/styles/styles/dark.qss" : ":/styles/styles/light.qss";
    QFile file(stylePath);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        stylePath = enable ? ":/styles/dark.qss" : ":/styles/light.qss";
        file.setFileName(stylePath);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
             qCritical() << "ERROR: Could not find style file:" << stylePath;
             return;
        }
    }
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
    file.close();
}

void SettingsDialog::setupStatisticsChart()
{
    ui->statsPlotWidget->clearPlottables();

    if (ui->statsPlotWidget->plotLayout()->elementCount() > 1) {
        ui->statsPlotWidget->plotLayout()->removeAt(0);
        ui->statsPlotWidget->plotLayout()->simplify();
    }

    ui->statsPlotWidget->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(ui->statsPlotWidget, tr("Історія активності"), QFont("Segoe UI", 12, QFont::Bold));
    if (isDarkThemeActive) title->setTextColor(Qt::white);
    else title->setTextColor(Qt::black);

    ui->statsPlotWidget->plotLayout()->addElement(0, 0, title);

    QMap<QString, int> stats = m_repository->getUsageStats();

    QString today = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    if (!stats.contains(today)) stats[today] = 0;

    QVector<double> ticks;
    QVector<QString> labels;
    QVector<double> values;

    int i = 1;
    for(auto it = stats.begin(); it != stats.end(); ++it) {
        ticks << i;
        QString shortDate = QDate::fromString(it.key(), "yyyy-MM-dd").toString("dd.MM");
        labels << shortDate;

        double minutes = it.value() / 60.0;
        values << minutes;
        i++;
    }

    QCPBars *bars = new QCPBars(ui->statsPlotWidget->xAxis, ui->statsPlotWidget->yAxis);
    bars->setName(tr("Активність (хв)"));
    bars->setPen(Qt::NoPen);
    bars->setBrush(QColor("#ff69b4"));

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);

    ui->statsPlotWidget->xAxis->setTicker(textTicker);
    ui->statsPlotWidget->xAxis->setLabel(tr("Дата"));
    ui->statsPlotWidget->xAxis->setTickLabelRotation(0);
    ui->statsPlotWidget->xAxis->setRange(0, ticks.size() + 1);

    ui->statsPlotWidget->yAxis->setLabel(tr("Час (хвилини)"));

    bars->setData(ticks, values);
    ui->statsPlotWidget->rescaleAxes();

    if (ui->statsPlotWidget->yAxis->range().upper < 10)
        ui->statsPlotWidget->yAxis->setRangeUpper(10);
    else
        ui->statsPlotWidget->yAxis->setRangeUpper(ui->statsPlotWidget->yAxis->range().upper * 1.2);

    ui->statsPlotWidget->replot();
}
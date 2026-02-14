#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"
#include "qcustomplot.h"
#include "statistics_service.h"

StatisticsDialog::StatisticsDialog(NoteRepository* repository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatisticsDialog),
    m_repository(repository)
{
    ui->setupUi(this);
    setWindowTitle(tr("Статистика нотаток"));
    resize(600, 400);

    setupChart();
}

StatisticsDialog::~StatisticsDialog() {
    delete ui;
}

void StatisticsDialog::setupChart() {

    QMap<QString, int> counts = StatisticsService::getNotesCountByCategory(m_repository);

    QCPBars *bars = new QCPBars(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    bars->setName(tr("Кількість нотаток"));
    bars->setAntialiased(false);
    bars->setPen(QPen(QColor(255, 105, 180).lighter(130)));
    bars->setBrush(QColor(255, 105, 180));

    QVector<double> ticks;
    QVector<QString> labels;
    QVector<double> values;

    int i = 1;
    for(auto it = counts.begin(); it != counts.end(); ++it) {
        ticks << i;
        labels << it.key();
        values << it.value();
        i++;
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->plotWidget->xAxis->setTicker(textTicker);
    ui->plotWidget->xAxis->setTickLabelRotation(60);
    ui->plotWidget->xAxis->setSubTicks(false);
    ui->plotWidget->xAxis->setTickLength(0, 4);
    ui->plotWidget->xAxis->setRange(0, ticks.size() + 1);
    ui->plotWidget->xAxis->setLabel(tr("Категорії (Схеми)"));

    ui->plotWidget->yAxis->setRange(0, 10);
    ui->plotWidget->yAxis->setPadding(5);
    ui->plotWidget->yAxis->setLabel(tr("Кількість"));
    ui->plotWidget->yAxis->grid()->setSubGridVisible(true);

    bars->setData(ticks, values);

    ui->plotWidget->rescaleAxes();
    if (ui->plotWidget->yAxis->range().upper < 1.0) {
       ui->plotWidget->yAxis->setRangeUpper(5.0);
    } else {
       ui->plotWidget->yAxis->setRangeUpper(ui->plotWidget->yAxis->range().upper * 1.2);
    }

    ui->plotWidget->replot();
}
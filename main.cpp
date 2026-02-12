#include <QApplication>
#include "mainwindow.h"
#include <QObject>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QFile>
#include <QTextStream>
#include "logger.h"


int main(int argc, char *argv[]) {
    Logger::init();

    QApplication a(argc, argv);

    QFile file(":/styles/styles/light.qss");

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        qApp->setStyleSheet(stream.readAll());
        file.close();
        qInfo() << "Стартова тема (light.qss) успішно завантажена.";
    } else {
        qWarning() << "НЕ ВДАЛОСЯ завантажити стартову тему! Перевір шлях у qrc.";
    }

    QTranslator translator;
    QString locale = QLocale::system().name();
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + locale,
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        a.installTranslator(&qtTranslator);
    }
    QString translationFileName = "lab3_" + locale;
    if (translator.load(translationFileName, "../i18n")) {
        a.installTranslator(&translator);
        qDebug() << "Завантажено переклад програми для локалізації:" << locale;
    } else {
        qDebug() << "Не вдалося завантажити переклад програми для локалізації:" << locale;
    }

    qInfo() << QObject::tr("Програма запущена.");
    MainWindow w;

    w.updateNotesList();

    w.show();
    int result = a.exec();
    qInfo() << QObject::tr("Застосунок завершено з кодом виходу %1").arg(result);
    return result;
}
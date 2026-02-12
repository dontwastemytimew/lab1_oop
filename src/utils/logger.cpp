#include "logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QObject>
#include <QDebug>


void Logger::init() {
    qInstallMessageHandler(Logger::messageHandler);
}


void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QFile file("log.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    QString logMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type) {
        case QtDebugMsg:    logMessage += QObject::tr("[DEBUG] ");   break;
        case QtInfoMsg:     logMessage += QObject::tr("[INFO] ");    break;
        case QtWarningMsg:  logMessage += QObject::tr("[WARNING] "); break;
        case QtCriticalMsg: logMessage += QObject::tr("[CRITICAL] ");break;
        case QtFatalMsg:    logMessage += QObject::tr("[FATAL] ");   break;
    }

    logMessage += msg;
    out << logMessage << "\n";
    file.close();
}
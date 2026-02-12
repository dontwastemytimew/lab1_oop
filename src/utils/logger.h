#pragma once

#include <QString>
#include <QMessageLogContext>
#include <QtGlobal>
#include <QDebug>

/**
 * @class Logger
 * @brief Статичний клас для налаштування системи логування.
 *
 * Відповідає за перехоплення всіх повідомлень qDebug/qInfo/qWarning
 * та їх перенаправлення у файл log.txt.
 */
class Logger {
public:
    /**
     * @brief Ініціалізує та встановлює глобальний обробник логів.
     * Цей метод має бути викликаний один раз на самому початку програми.
     */
    static void init();

private:
 /**
  * @brief Кастомний обробник повідомлень (message handler).
  *
  * Ця функція викликається Qt для кожного лог-повідомлення.
  * Вона форматує рядок (додає дату, час, рівень) та записує його
  * у файл "log.txt".
  *
  * @param type Тип повідомлення (Debug, Info, Warning, Critical).
  * @param context Контекст повідомлення (файл, рядок).
  * @param msg Текст самого повідомлення.
  */
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};
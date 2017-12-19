#ifndef ESPTOUCH_H
#define ESPTOUCH_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QPointer>
#include "esptouchtask.h"

class Esptouch : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool running READ isRunning NOTIFY isRunningChanged)

    QString apSsid;
    QString apBssid;
    QString apPassword;
    bool running;
    QPointer<EspTouchTask> pTask;
    bool isRunning();
public:
    Esptouch(QObject *parent = 0);
signals:
    void started();
    void completed();
    void isRunningChanged();
public slots:
    void interrupt();
    void execute(const QString &apSsid, const QString &apBssid, const QString &apPassword);
};

#endif // ESPTOUCH_H

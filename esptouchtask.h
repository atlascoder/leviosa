#ifndef ESPTOUCHTASK_H
#define ESPTOUCHTASK_H

#include <QThread>
#include <QString>

class EspTouchTask : public QThread
{
    Q_OBJECT
    QString apSsid;
    QString apBssid;
    QString apPassword;
    QString response;

    void run() override;
public:
    EspTouchTask(const QString &apSsid, const QString &apBssid, const QString &apPassword):
        apSsid(apSsid), apBssid(apBssid), apPassword(apPassword) {}

signals:
    void finished(QString &espTouchReult);

public slots:
    void interrupt();
};

#endif // ESPTOUCHTASK_H

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H
#include <QObject>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QList>
#include <QNetworkSession>
#include <QPointer>

class NetworkMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString onlineState READ getState NOTIFY currentStateChanged)
    QNetworkConfigurationManager mgr;
    QString m_state;
    QPointer<QNetworkSession> session;
    QString getState() const { return m_state; }
    void changeState(bool isConnected);
signals:
    void changed();
    void currentStateChanged(const QString &state);
public slots:
    void isOnline(const bool connected);
public:
    NetworkMonitor(QObject *parent = 0);
};

#endif // NETWORKMONITOR_H

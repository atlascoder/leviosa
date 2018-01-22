#ifndef WLANAPI_H
#define WLANAPI_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class WlanAPI : public QObject
{
    Q_OBJECT

    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;

public:
    WlanAPI(QObject *parent = 0);
    virtual ~WlanAPI();

    void shadeCmd(const QString &ip, char channel, int cmd);

public slots:
    void reqFinished();

signals:
    void reqSuccessful();
    void reqFailed();

private:
    void requestGet(const QString &url);

};

#endif // WLANAPI_H

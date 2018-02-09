#ifndef WANAPI_H
#define WANAPI_H

#include <QString>
#include <QObject>

class WanAPI : public QObject
{
    Q_OBJECT

public:
    static WanAPI& instance() {
        static WanAPI sInstance;
        return sInstance;
    }

    virtual ~WanAPI();

    void shadeCmd(const QString &mac, char channel, int cmd);

public slots:
    void reqFinished();

signals:
    void reqSuccessful();
    void reqFailed();

private:
    WanAPI(QObject *parent = 0);

};

#endif // WANAPI_H

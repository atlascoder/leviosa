#ifndef REPLYTIMEOUT_H
#define REPLYTIMEOUT_H

#include <QObject>
#include <QBasicTimer>
#include <QNetworkReply>

class ReplyTimeout : public QObject
{
    Q_OBJECT
    QBasicTimer m_timer;
public:
    enum HandleMethod { Abort, Close };
    ReplyTimeout(QNetworkReply* reply, const int timeout, HandleMethod method = Abort);
    static void set(QNetworkReply* reply, const int timeout, HandleMethod method = Abort);
protected:
    void timerEvent(QTimerEvent * ev);
private:
    HandleMethod m_method;
};

#endif // REPLYTIMEOUT_H

#include "ReplyTimeout.h"
#include <QTimerEvent>

ReplyTimeout::ReplyTimeout(QNetworkReply *reply, const int timeout, HandleMethod method) : QObject(reply), m_method(method)
{
    Q_ASSERT(reply);
    if (reply && reply->isRunning()) {
        m_timer.start(timeout, this);
        connect(reply, &QNetworkReply::finished, this, &QObject::deleteLater);
    }
}
void ReplyTimeout::set(QNetworkReply* reply, const int timeout, HandleMethod method)
{
    new ReplyTimeout(reply, timeout, method);
}

void ReplyTimeout::timerEvent(QTimerEvent *event)
{
    if (!m_timer.isActive() || event->timerId() != m_timer.timerId())
        return;

    auto reply = static_cast<QNetworkReply*>(parent());
    if (reply->isRunning()) {
        if (m_method == Close)
            reply->close();
        else if (m_method == Abort)
            reply->abort();
        m_timer.stop();
    }
}

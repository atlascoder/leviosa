#ifndef USERLOCATION_H
#define USERLOCATION_H

#include <QString>
#include <QUuid>
#include <QJsonObject>
#include "Positioned.h"
#include "Syncable.h"

class Location : public Positioned, public Syncable
{
    QString mBssid;
    QString mName;
    QString mTimezone;
    bool mIsOnWlan;
    bool mIsOnline;
public:
    Location();
    Location(const QUuid & uuid);
    Location(const QString & uuidStr);

    QString bssid() const;
    void setBssid(const QString &bssid);

    QString name() const;
    void setName(const QString &name);

    QString timezone() const;
    void setTimezone(const QString &tzone);

    bool isOnWlan() const;
    void setIsOnWlan(bool onWlan);

    bool isOnline() const;
    void setOnline(bool isOnline);

    virtual QJsonObject toJson() const;
    virtual void withJson(const QJsonObject& json);
};

#endif // USERLOCATION_H

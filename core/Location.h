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
    int mUtcOffset;
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

    int utcOffset() const;
    void setUtcOffset(int offset);

    bool isOnWlan() const;
    void setIsOnWlan(bool onWlan);

    bool isOnline() const;
    void setOnline(bool isOnline);

    virtual QJsonObject toJson() const;
    virtual void withJson(const QJsonObject& json);
};

#endif // USERLOCATION_H
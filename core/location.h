#ifndef USERLOCATION_H
#define USERLOCATION_H

#include <QString>
#include <QUuid>
#include <QJsonObject>
#include "positioned.h"
#include "syncable.h"

class Location : public Positioned, public Syncable
{
    QUuid mUuid;
    QString mBssid;
    QString mName;
    int mUtcOffset;
    bool mIsOnWlan;
    bool mIsOnline;
public:
    Location();
    explicit Location(const QUuid & uuid);
    explicit Location(const QString & uuidStr);

    QUuid uuid() const { return mUuid; }
    void setUuid(const QUuid & uuid) { mUuid = uuid; }

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

    QJsonObject toJson() const;
    void withJson(const QJsonObject & json);

};

#endif // USERLOCATION_H

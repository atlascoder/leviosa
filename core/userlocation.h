#ifndef USERLOCATION_H
#define USERLOCATION_H

#include <QString>
#include <QUuid>
#include <QJsonObject>
#include "positioned.h"

class UserLocation : public Positioned
{
    QUuid mUuid;
    QString mBssid;
    QString mSsid;
    QString mName;
    int mUtcOffset;
    int mLastModified;
    bool mIsOnWlan;
    bool mIsOnline;
public:
    UserLocation();
    explicit UserLocation(const QUuid & uuid);
    explicit UserLocation(const QString & uuidStr);

    QUuid uuid() const { return mUuid; }
    void setUuid(const QUuid & uuid) { mUuid = uuid; }

    QString bssid() const;
    void setBssid(const QString &bssid);

    QString name() const;
    void setName(const QString &name);

    QString ssid() const;
    void setSsid(const QString &ssid);

    int utcOffset() const;
    void setUtcOffset(int offset);

    int lastModified() const { return mLastModified; }
    void setLastModified(int lastModified) { mLastModified = lastModified; }

    bool isOnWlan() const;
    void setIsOnWlan(bool onWlan);

    bool isOnline() const;
    void setOnline(bool isOnline);

    QJsonObject toJson() const;
    void withJson(const QJsonObject & json);

};

#endif // USERLOCATION_H

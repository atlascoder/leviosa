#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QString>
#include <QJsonObject>
#include "shade.h"
#include "schedule.h"
#include "positioned.h"
#include "Syncable.h"

class LocationController: public Shade, public Schedule, public Positioned
{
    QString mMac;
    QString mLocationUuid;
    QString mName;
    bool mIsWLAN;
    QString mIpAddress;
    int mLastModified;
public:
    LocationController() : mLastModified(0) {}
    explicit LocationController(const QString& mac);

    QString mac() const;
    void setMac(const QString& mac);

    QString locationUuid() const;
    void setLocationUuid(const QString& locationUuid);

    QString name() const;
    void setName(const QString &name);

    bool isWlan() const;
    void setIsWlan(bool isWlan);

    QString ipAddress() const;
    void setIpAddress(const QString& ipAddress);

    int lastModified() const { return mLastModified; }
    void setLastModified(int lastModified) { mLastModified = lastModified; }

    QJsonObject toJson() const;
    void withJson(const QJsonObject& json);

};

#endif // LOCATIONCONTROLLER_H

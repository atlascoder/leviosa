#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QString>
#include <QJsonObject>
#include "shade.h"
#include "schedule.h"
#include "positioned.h"
#include "syncable.h"
#include "SyncableRecord.h"

class Controller: public Shade, public Positioned, public Syncable
{
    QString mMac;
    QString mLocationUuid;
    QString mName;
    bool mIsWLAN;
    QString mIpAddress;
public:
    Controller() {}
    Controller(const Controller& c) :
        Positioned(c),
        Syncable(c),
        mMac(c.mMac),
        mLocationUuid(c.mLocationUuid),
        mName(c.mName),
        mIsWLAN(c.mIsWLAN),
        mIpAddress(c.mIpAddress)
    {}

    explicit Controller(const QString& mac);

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

    QJsonObject toJson() const;
    void withJson(const QJsonObject& json);

};

#endif // LOCATIONCONTROLLER_H

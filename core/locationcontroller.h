#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QString>
#include "shade.h"
#include "schedule.h"
#include "positioned.h"
#include "Syncable.h"

class LocationController: public Shade, public Schedule, public Positioned, public Syncable
{
    int mLocationId;
    QString mName;
    QString mMac;
    bool mIsWLAN;
    QString mIpAddress;
public:
    explicit LocationController(int locationId = 0);

    int locationId() const;
    void setLocationId(int locationId);

    QString name() const;
    void setName(const QString &name);

    QString mac() const;
    void setMac(const QString& mac);

    bool isWlan() const;
    void setIsWlan(bool isWlan);

    QString ipAddress() const;
    void setIpAddress(const QString& ipAddress);

};

#endif // LOCATIONCONTROLLER_H

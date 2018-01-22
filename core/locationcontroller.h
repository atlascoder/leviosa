#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QString>

class LocationController
{
    int mId;
    int mLocationId;
    QString mName;
    QString mMac;
    int mPosition;
public:
    explicit LocationController(int locationId = 0);

    int id() const;
    void setId(const int id);

    int locationId() const;
    void setLocationId(int locationId);

    QString name() const;
    void setName(const QString &name);

    QString mac() const;
    void setMac(const QString& mac);

    int position() const;
    void setPosition(int position);
};

#endif // LOCATIONCONTROLLER_H

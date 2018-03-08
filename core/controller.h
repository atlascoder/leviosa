#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QString>
#include <QJsonObject>
#include "shade.h"
#include "schedule.h"
#include "positioned.h"
#include "syncable.h"
#include "SyncableRecord.h"
#include "ControllerAPI.h"
#include <memory>

class Controller: public Shade, public Positioned, public Syncable
{
    QString mMac;
    QString mLocationUuid;
    QString mName;
    bool mMatchPrevious;
//    std::unique_ptr<ControllerAPI> mControllerAPI;
public:
    Controller() //: mControllerAPI(new ControllerAPI)
    {}
    Controller(const Controller& c) :
        Positioned(c),
        Syncable(c),
        mMac(c.mMac),
        mLocationUuid(c.mLocationUuid),
        mName(c.mName)//,
//        mControllerAPI(new ControllerAPI)
    {
//        setIpAddress(c.ipAddress());
    }
    ~Controller(){}

    explicit Controller(const QString& mac);

    QString mac() const;
    void setMac(const QString& mac);

    QString locationUuid() const;
    void setLocationUuid(const QString& locationUuid);

    QString name() const;
    void setName(const QString &name);

//    bool onWlan() const;
//    void setOnWlan(bool onWlan);

//    QString ipAddress() const;
//    void setIpAddress(const QString& ipAddress);

    QJsonObject toJson() const;
    void withJson(const QJsonObject& json);

    bool matchPrevious() const { return mMatchPrevious; }
    void setMatchPrevious(bool isMatch) { mMatchPrevious = isMatch; }

//    ControllerAPI& api() const { return *mControllerAPI.get(); }

};

#endif // LOCATIONCONTROLLER_H

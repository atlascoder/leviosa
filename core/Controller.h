#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QString>
#include <QJsonObject>
#include "Shade.h"
#include "Schedule.h"
#include "Positioned.h"
#include "Syncable.h"

class Controller: public Shade, public Positioned, public Syncable
{
    QString mMac;
    QString mLocationUuid;
    QString mName;
    int mId;
    bool mMatchPrevious;
public:
    Controller():
        mMac("0000000000"),
        mLocationUuid(""),
        mName("A Zone"),
        mId(1),
        mMatchPrevious(false)
    { }

    Controller(const Controller& c) :
        Positioned(c),
        Syncable(c),
        mMac(c.mMac),
        mLocationUuid(c.mLocationUuid),
        mName(c.mName),
        mId(c.mId)
    { }

    Controller(const QString& uuid);

    QString mac() const;
    void setMac(const QString& mac);

    QString locationUuid() const;
    void setLocationUuid(const QString& locationUuid);

    QString name() const;
    void setName(const QString &name);

    int id() const;
    void setId(int id);

    virtual QJsonObject toJson() const;
    virtual void withJson(const QJsonObject& json);

    bool matchPrevious() const { return mMatchPrevious; }
    void setMatchPrevious(bool isMatch) { mMatchPrevious = isMatch; }

};

#endif // LOCATIONCONTROLLER_H

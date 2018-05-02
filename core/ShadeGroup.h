#ifndef SHADESGROUP_H
#define SHADESGROUP_H

#include <QString>
#include <QJsonObject>
#include "Shade.h"
#include "Schedule.h"
#include "Positioned.h"
#include "Syncable.h"

class ShadeGroup : public Syncable, public Shade, public Schedule, public Positioned
{
    char mChannel;
    QString mControllerMac;
    QString mName;
public:
    ShadeGroup(const QString& uuid);
    ShadeGroup(char channel);
    ShadeGroup();
    ShadeGroup(const ShadeGroup& shadeGroup) :
        Syncable(shadeGroup), Shade(), Schedule(shadeGroup), Positioned(shadeGroup),
        mChannel(shadeGroup.mChannel),
        mControllerMac(shadeGroup.mControllerMac),
        mName(shadeGroup.mName)
    {}

    char channel() const;
    void setChannel(char channel);

    QString controllerMac() const;
    void setControllerMac(const QString& controllerMac);

    QString name() const;
    void setName(const QString& name);

    virtual QJsonObject toJson() const;
    virtual void withJson(const QJsonObject& json);

};

#endif // SHADESGROUP_H

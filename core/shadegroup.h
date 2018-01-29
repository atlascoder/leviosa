#ifndef SHADESGROUP_H
#define SHADESGROUP_H

#include <QString>
#include <QJsonObject>
#include "shade.h"
#include "schedule.h"
#include "positioned.h"
#include "syncable.h"

class ShadeGroup : public Shade, public Schedule, public Positioned, public Syncable
{
    char mChannel;
    QString mControllerMac;
    QString mName;
public:
    ShadeGroup(char channel);
    ShadeGroup();
    ShadeGroup(const ShadeGroup& shadeGroup) :
        Schedule(shadeGroup), Positioned(shadeGroup), Syncable(shadeGroup),
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

    QJsonObject toJson() const;
    void withJson(const QJsonObject& json);
};

#endif // SHADESGROUP_H

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
    bool mMatchPrevious;
public:
    ShadeGroup(char channel);
    ShadeGroup();
    ShadeGroup(const ShadeGroup& shadeGroup) :
        Schedule(shadeGroup), Positioned(shadeGroup), Syncable(shadeGroup),
        mChannel(shadeGroup.mChannel),
        mControllerMac(shadeGroup.mControllerMac),
        mName(shadeGroup.mName)
    {}

    bool operator == (const ShadeGroup& b) const {
        return mChannel==b.mChannel
                && mControllerMac==b.mControllerMac
                && mName==b.mName
                && position()==b.position()
                && openAt() == b.openAt()
                && closeAt() == b.closeAt()
                && days() == b.days();
    }

    char channel() const;
    void setChannel(char channel);

    QString controllerMac() const;
    void setControllerMac(const QString& controllerMac);

    QString name() const;
    void setName(const QString& name);

    QJsonObject toJson() const;
    void withJson(const QJsonObject& json);

    bool matchPrevious() const { return mMatchPrevious; }
    void setMatchPrevious(bool isMatch) { mMatchPrevious = isMatch; }
};

#endif // SHADESGROUP_H

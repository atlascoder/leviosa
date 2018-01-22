#ifndef SHADESGROUP_H
#define SHADESGROUP_H

#include <QString>
#include "shade.h"
#include "schedule.h"
#include "positioned.h"
#include "Syncable.h"

class ShadeGroup : public Shade, public Schedule, public Positioned, public Syncable
{
    int mControllerId;
    char mChannel;
    QString mName;
public:
    ShadeGroup(int controllerId);
    ShadeGroup();

    int controllerId() const;
    void setControllerId(int controllerId);

    QString name() const;
    void setName(const QString& name);

    char channel() const;
    void setChannel(char channel);
};

#endif // SHADESGROUP_H

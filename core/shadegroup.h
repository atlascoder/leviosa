#ifndef SHADESGROUP_H
#define SHADESGROUP_H

#include <QString>
#include "shade.h"
#include "schedule.h"

class ShadesGroup : public Shade, public Schedule
{
    int mId;
    int mControllerId;
    QString mName;
    int mPosition;
public:
    explicit ShadesGroup(int controllerId);

    int id() const;
    void setId(int id);

    int controllerId() const;
    void setControllerId(int controllerId);

    QString name() const;
    void setName(const QString& name);

    int position() const;
    void setPosition(int position);
};

#endif // SHADESGROUP_H

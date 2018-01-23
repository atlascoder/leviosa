#ifndef SHADESGROUP_H
#define SHADESGROUP_H

#include <QString>
#include <QJsonObject>
#include "shade.h"
#include "schedule.h"
#include "positioned.h"
#include "Syncable.h"

class ShadeGroup : public Shade, public Schedule, public Positioned
{
    char mChannel;
    QString mControllerMac;
    QString mName;
    int mLastModified;
public:
    ShadeGroup(char channel);
    ShadeGroup();

    char channel() const;
    void setChannel(char channel);

    QString controllerMac() const;
    void setControllerMac(const QString& controllerMac);

    QString name() const;
    void setName(const QString& name);

    int lastModified() const { return mLastModified; }
    void setLastModified(int lastModified) { mLastModified = lastModified; }

    QJsonObject toJson() const;
    void withJson(const QJsonObject& json);
};

#endif // SHADESGROUP_H

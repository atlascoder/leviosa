#ifndef CONTROLLERSCHEDULE_H
#define CONTROLLERSCHEDULE_H

#include <QString>
#include <QJsonObject>
#include "Schedule.h"

class ControllerSchedule
{
public:
    static const int GROUPS_COUNT = 6;
    ControllerSchedule();

    ControllerSchedule& operator=(const ControllerSchedule& other);

    QString json() const;
    bool parseJson(const QJsonObject& json);

    QString timezone;
    Schedule schedules[GROUPS_COUNT];

    void clean();
};

#endif // CONTROLLERSCHEDULE_H

#ifndef SCHEDULELINE_H
#define SCHEDULELINE_H

#include <QString>

struct ScheduleLine {
    int dayEU;
    int hour;
    int minute;
    QString command;
    int channel;

    QString asQueryParams() const {
        return QString("&sch_wday=")
                .append(QString::number(dayEU))
                .append("&sch_hour=")
                .append(QString::number(hour))
                .append("&sch_minute=")
                .append(QString::number(minute))
                .append("&sch_command=")
                .append(command)
                .append("&sch_channel=")
                .append(QString::number(channel));
    }
};

#endif // SCHEDULELINE_H

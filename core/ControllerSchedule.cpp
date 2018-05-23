#include "ControllerSchedule.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDebug>

ControllerSchedule::ControllerSchedule():
    timezone("EST")
{ }

ControllerSchedule& ControllerSchedule::operator=(const ControllerSchedule& other)
{
    if (this != &other) {
        this->timezone = other.timezone;
        for (int i = 0; i < GROUPS_COUNT; i++) {
            this->schedules[i].setOpenAt(other.schedules[i].openAt());
            this->schedules[i].setCloseAt(other.schedules[i].closeAt());
            this->schedules[i].setDays(other.schedules[i].days());
        }
    }
    return *this;
}

void ControllerSchedule::clean()
{
    for (int i = 0; i < GROUPS_COUNT; i++) {
        schedules[i].clean();
    }
}

QString ControllerSchedule::json() const
{
    QJsonDocument doc;
    QJsonObject root_obj;
    root_obj.insert("timeZone", timezone);
    QJsonArray schedules_arr;
    for (int i = 0; i < GROUPS_COUNT; i++) {
        if (schedules[i].days() > 0) {
            QJsonArray sched_arr;
            sched_arr.append(QJsonValue(i));
            sched_arr.append(QJsonValue(schedules[i].openAt()));
            sched_arr.append(QJsonValue(schedules[i].closeAt()));
            sched_arr.append(QJsonValue(schedules[i].days()));
            schedules_arr.append(sched_arr);
        }
    }

    root_obj.insert("schedule", schedules_arr);

    return QJsonDocument(root_obj).toJson(QJsonDocument::Compact);
}

bool ControllerSchedule::parseJson(const QJsonObject &json)
{
    ControllerSchedule temp_schedule;
    if (json.contains("timeZone") && json.value("timeZone").isString()) {
        temp_schedule.timezone = json.value("timeZone").toString();
    }
    else {
        qDebug() << "`timeZone` should be present";
        return false;
    }

    if (json.contains("schedule") && json.value("schedule").isArray()) {
        QJsonArray arr = json.value("schedule").toArray();
        if (arr.count() > GROUPS_COUNT) {
            qDebug() << "`schedule` array is too big n=" << arr.count() << " is more that max allowed " << GROUPS_COUNT;
            return false;
        }
        for (QJsonValue obj: arr) {
            if (obj.isArray() && obj.toArray().count() == 4) {
                QJsonArray a = obj.toArray();
                if (a[0].isDouble() && a[1].isDouble() && a[2].isDouble() && a[3].isDouble()) {

                    int n = a[0].toInt();
                    if (n < 0 || n >= GROUPS_COUNT) {
                        qDebug() << "schedule item #0 should be from 0 to " << GROUPS_COUNT - 1;
                        return false;
                    }

                    int open_at = a[1].toInt();
                    if (open_at < 0 || open_at >= 1440) {
                        qDebug() << "schedule item #1 should be from 0 to " << 1439;
                        return false;
                    }

                    int close_at = a[2].toInt();
                    if (close_at < 0 || close_at >= 1440) {
                        qDebug() << "schedule item #2 should be from 0 to " << 1439;
                        return false;
                    }

                    int days = a[3].toInt();
                    if (days < 0 || days > 255) {
                        qDebug() << "schedule item #3 should be from 0 to " << 255;
                        return false;
                    }
                    temp_schedule.schedules[n].setOpenAt(open_at);
                    temp_schedule.schedules[n].setCloseAt(close_at);
                    temp_schedule.schedules[n].setDays(days);
                }
                else {
                    qDebug() << "`schedule` array should contain only arrays of 4 integers";
                    return false;
                }

            }
            else {
                qDebug() << "`schedule` array should contain only arrays of 4 integers";
                return false;
            }
        }
    }
    else {
        qDebug() << "`schedule` should be an array of values";
        return false;
    }

    *this = temp_schedule;
    return true;
}

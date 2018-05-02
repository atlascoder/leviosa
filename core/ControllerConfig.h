#ifndef CONTROLLERCONFIG_H
#define CONTROLLERCONFIG_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <memory>
#include <vector>
#include "ScheduleLine.h"

using namespace std;

class ControllerConfig
{
    QString mJsonized;
    bool mIsValid;
    QString mVersion;
    QString mMac;
    QStringList mRfChannels;
    QString mTimezone;
    QDateTime mTime;
    unique_ptr<vector<unique_ptr<ScheduleLine>>> mScheduleLines;

   QString mScheduleJsonString;
public:
    ControllerConfig() : mScheduleLines(new std::vector<std::unique_ptr<ScheduleLine>>) {}
    ControllerConfig& operator=(const ControllerConfig&);

    void parse(const QString& jsonized);
    unique_ptr<ScheduleLine> buildScheduleLine(const QJsonObject& obj) const;
    vector<unique_ptr<ScheduleLine>>& scheduleLines() { return *mScheduleLines.get(); }

    QString mac() const {return mMac; }

    bool isValid() const { return mIsValid; }

    char days(int channel) const;

    int openAt(int channel) const;

    int closeAt(int channel) const;

    void addSchedule(int channel, int day, int openAt, int closeAt);

    void clearSchedule();

    QString timezone() const { return mTimezone; }
    void setTimezone(const QString& timezone) { mTimezone = timezone; }
    QDateTime dateTime() const { return mTime; }

    QString scheduleJsonString() const { return mScheduleJsonString; }

    int scheduledChannels() const { return mScheduleLines->size() / 2; }

    static int utcOffset(const QString& abbr);
    static char daysUStoEU(char usDays);
    static char daysEUtoUS(char days);

};

#endif // CONTROLLERCONFIG_H

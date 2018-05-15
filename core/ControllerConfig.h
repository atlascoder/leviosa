#ifndef CONTROLLERCONFIG_H
#define CONTROLLERCONFIG_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <memory>
#include <vector>

#include "ControllerSchedule.h"

using namespace std;

class ControllerConfig
{
    QString mJsonized;
    bool mIsValid;
    QString mVersion;
    QString mMac;
    QStringList mRfChannels;
    QDateTime mTime;
    shared_ptr<ControllerSchedule> mControllerSchedule;

public:
    ControllerConfig() : mControllerSchedule(new ControllerSchedule) {}
    ControllerConfig& operator=(const ControllerConfig&);

    void parse(const QString& jsonized);
    shared_ptr<ControllerSchedule> schedule() const { return mControllerSchedule; }


    QString mac() const {return mMac; }

    bool isValid() const { return mIsValid; }

    char days(int channel) const;

    int openAt(int channel) const;

    int closeAt(int channel) const;

    void setGroupSchedule(int channel, int day, int openAt, int closeAt);

    void cleanSchedule();

    QString timezone() const { return mControllerSchedule->timezone; }
    void setTimezone(const QString& timezone) { mControllerSchedule->timezone = timezone; }
    QDateTime dateTime() const { return mTime; }

    static int utcOffset(const QString& abbr);
    static char daysUStoEU(char usDays);
    static char daysEUtoUS(char days);

};

#endif // CONTROLLERCONFIG_H

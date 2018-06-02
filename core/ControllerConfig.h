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
    ControllerConfig() : mIsValid(false), mControllerSchedule(new ControllerSchedule) {}
    ControllerConfig& operator=(const ControllerConfig&);
    bool operator==(const ControllerConfig& other) const;
    bool operator!=(const ControllerConfig& other) const;

    void parse(const QString& jsonized);
    shared_ptr<ControllerSchedule> schedule() const { return mControllerSchedule; }


    QString mac() const { return mMac; }
    void setMac(const QString& mac) { mMac = mac; }

    bool isValid() const { return mIsValid; }
    void setValid(bool isValid) { mIsValid = isValid; }

    char days(int channel) const;

    int openAt(int channel) const;

    int closeAt(int channel) const;

    void setGroupSchedule(int group, int day, int openAt, int closeAt);

    void cleanSchedule();

    QString timezone() const { return mControllerSchedule->timezone; }
    void setTimezone(const QString& timezone) { mControllerSchedule->timezone = timezone; }
    QDateTime dateTime() const { return mTime; }

    static int utcOffset(const QString& abbr);
    static char daysUStoEU(char usDays);
    static char daysEUtoUS(char days);

};

#endif // CONTROLLERCONFIG_H

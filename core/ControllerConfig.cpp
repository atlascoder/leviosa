#include "ControllerConfig.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QByteArray>
#include <QTimeZone>
#include <QLocale>

#include <QDebug>

using namespace std;

ControllerConfig& ControllerConfig::operator=(const ControllerConfig &config)
{
    mIsValid = config.mIsValid;
    mVersion = config.mVersion;
    mMac = config.mMac;
    mTime = config.mTime;
    mControllerSchedule = config.mControllerSchedule;
    mJsonized = config.mJsonized;
    return *this;
}

int ControllerConfig::utcOffset(const QString& abbr)
{
    if(abbr == "EST")   return -18000;
    if(abbr == "CST")   return -21600;
    if(abbr == "MST")   return -25200;
    if(abbr == "PST")   return -28800;
    if(abbr == "AKST")  return -32400;
    if(abbr == "HST")   return -36000;
    return 0;
}

void ControllerConfig::parse(const QString &jsonized)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(jsonized.toStdString()), &error);
    mIsValid = false;
    if((error.error == QJsonParseError::NoError) && doc.isObject()){
        QJsonObject obj = doc.object();

        if(obj.contains("version"))
            mVersion = obj.value("version").toString();
        else{
            qDebug() << "Controller config failed: version not found";
            return;
        };

        if(obj.contains("mac_address"))
            mMac = obj.value("mac_address").toString();
        else{
            qDebug() << "Controller config failed: mac_address not found";
            return;
        };

        QLocale loc(QLocale::English);
        if(obj.contains("time")){
            mTime = loc.toDateTime(obj.value("time").toString(), "ddd MMM d HH:mm:ss yyyy");
            if(!mTime.isValid())
                mTime = loc.toDateTime(obj.value("time").toString(), "ddd MMM  d HH:mm:ss yyyy");
        }
        else{
            qDebug() << "Controller config failed: time not found";
            return;
        };

        if(obj.contains("schedule") && obj.value("schedule").isObject()){
            mControllerSchedule->parseJson(obj.value("schedule").toObject());
        }
        else return;

        mIsValid = true;
    }
    else
        qDebug() << "Controller config failed: JSON parsing error " << error.errorString();
}

char ControllerConfig::days(int group) const
{
    if (group >= 0 && group < ControllerSchedule::GROUPS_COUNT)
        return daysEUtoUS(mControllerSchedule->schedules[group].days());
    else
        return 0;
}

int ControllerConfig::openAt(int group) const
{
    if (group >= 0 && group < ControllerSchedule::GROUPS_COUNT)
        return mControllerSchedule->schedules[group].openAt();
    else
        return 0;
}

int ControllerConfig::closeAt(int group) const
{
    if (group >= 0 && group < ControllerSchedule::GROUPS_COUNT)
        return mControllerSchedule->schedules[group].closeAt();
    else
        return 0;
}

char ControllerConfig::daysEUtoUS(char eu)
{
    if(eu&(1<<6)) return (eu<<1)^0b10000001;
    return eu<<1;
}

char ControllerConfig::daysUStoEU(char us)
{
    if(us&1) return (us >> 1)|0b1000000;
    else return us >> 1;
}

void ControllerConfig::setGroupSchedule(int group, int dayUS, int openAt, int closeAt)
{
    if (group < 0 || group >= ControllerSchedule::GROUPS_COUNT) return;
    mControllerSchedule->schedules[group].setOpenAt(openAt);
    mControllerSchedule->schedules[group].setCloseAt(closeAt);
    mControllerSchedule->schedules[group].setDays(daysUStoEU(dayUS));
}

void ControllerConfig::cleanSchedule()
{
    mControllerSchedule->clean();
}

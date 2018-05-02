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
    mScheduleLines->clear();
    mIsValid = config.mIsValid;
    mVersion = config.mVersion;
    mMac = config.mMac;
    mTimezone = config.mTimezone;
    mTime = config.mTime;
    mJsonized = config.mJsonized;
    mScheduleJsonString = config.mScheduleJsonString;

    for (unique_ptr<ScheduleLine>& line:*config.mScheduleLines) {
        unique_ptr<ScheduleLine> l(new ScheduleLine);
        l->channel = line->channel;
        l->open_mod = line->open_mod;
        l->close_mod = line->close_mod;
        l->dayEU = line->dayEU;
        mScheduleLines->push_back(move(l));
    }
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
            QJsonObject s_obj = obj.value("schedule").toObject();
            if (s_obj.contains("timeZone") && s_obj.value("timeZone").isString()) {
                mTimezone = s_obj.value("timeZone").toString();
            }
            else return;

            if (s_obj.contains("schedule") && s_obj.value("schedule").isArray()) {
                mScheduleLines->clear();
                QJsonArray s_arr = s_obj.value("schedule").toArray();
                for (int i = 0; i < s_arr.size(); i++) {
                    if (s_arr.at(i).isArray()) {
                        QJsonArray s_arr1 = s_arr.at(i).toArray();
                        if (s_arr1.size() == 4) {
                            unique_ptr<ScheduleLine> s(new ScheduleLine);
                            if (s_arr1.at(0).isDouble()) {
                                s->channel = s_arr1.at(0).toInt();
                            }
                            else return;
                            if (s_arr1.at(1).isDouble()) {
                                s->open_mod = s_arr1.at(1).toInt();
                            }
                            else return;
                            if (s_arr1.at(2).isDouble()) {
                                s->close_mod = s_arr1.at(2).toInt();
                            }
                            else return;
                            if (s_arr1.at(3).isDouble()) {
                                s->dayEU = s_arr1.at(3).toInt();
                            }
                            else return;
                            mScheduleLines->push_back(move(s));
                        }
                        else return;
                    }
                    else return;
                }
            }
            else return;
        }
        else return;

        mIsValid = true;
    }
    else
        qDebug() << "Controller config failed: JSON parsing error " << error.errorString();
}

unique_ptr<ScheduleLine> ControllerConfig::buildScheduleLine(const QJsonObject &obj) const
{
    unique_ptr<ScheduleLine> l(new ScheduleLine);
    if(obj.contains("sch_wday")) l->dayEU = obj.value("sch_wday").toInt();
    if(obj.contains("sch_hour")) l->hour =obj.value("sch_hour").toInt();
    if(obj.contains("sch_minute")) l->minute = obj.value("sch_minute").toInt();
    if(obj.contains("sch_command")) l->command = obj.value("sch_command").toString();
    if(obj.contains("sch_channel")) l->channel = obj.value("sch_channel").toInt();
    return l;
}

char ControllerConfig::days(int channel) const
{
    vector<unique_ptr<ScheduleLine>>::iterator l = find_if(mScheduleLines->begin(), mScheduleLines->end(), [channel](const unique_ptr<ScheduleLine>& l)->bool{ return channel==l->channel; });
    if(l!=mScheduleLines->end()){
        return daysEUtoUS(l->get()->dayEU);
    }
    return 0;
}

int ControllerConfig::openAt(int channel) const
{
    vector<unique_ptr<ScheduleLine>>::iterator l = find_if(mScheduleLines->begin(), mScheduleLines->end(), [channel](const unique_ptr<ScheduleLine>& l)->bool{ return (channel==l->channel) && ("up"==l->command); });
    if(l != mScheduleLines->end()){
        return l->get()->hour*60 + l->get()->minute;
    }
    return 0;
}

int ControllerConfig::closeAt(int channel) const
{
    vector<unique_ptr<ScheduleLine>>::iterator l = find_if(mScheduleLines->begin(), mScheduleLines->end(), [channel](const unique_ptr<ScheduleLine>& l)->bool{ return (channel==l->channel) && ("down"==l->command); });
    if(l != mScheduleLines->end()){
        return l->get()->hour*60 + l->get()->minute;
    }
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

void ControllerConfig::addSchedule(int channel, int dayUS, int openAt, int closeAt)
{
    unique_ptr<ScheduleLine> l(new ScheduleLine);
    l->channel = channel;
    l->dayEU = daysUStoEU(dayUS);
    l->hour = openAt / 60;
    l->minute = openAt % 60;
    l->command = "up";
    mScheduleLines->push_back(move(l));
    l.reset(new ScheduleLine);
    l->channel = channel;
    l->dayEU = daysUStoEU(dayUS);
    l->hour = closeAt / 60;
    l->minute = closeAt % 60;
    l->command = "down";
    mScheduleLines->push_back(move(l));
}

void ControllerConfig::clearSchedule()
{
    mScheduleLines->clear();
}

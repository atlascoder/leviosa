#include "Schedule.h"

Schedule::Schedule():
    mDaysMask(0), mOpenAt(8*60), mCloseAt(19*60)
{

}

int Schedule::days() const
{
    return mDaysMask;
}

void Schedule::setDays(int days)
{
    mDaysMask = days;
}

int Schedule::openAt() const
{
    return mOpenAt;
}

void Schedule::setOpenAt(int minuteOfDay)
{
    mOpenAt = minuteOfDay;
}

int Schedule::openAtHour() const
{
    return mOpenAt / 60;
}

int Schedule::openAtMinute() const
{
    return mOpenAt % 60;
}

int Schedule::closeAt() const
{
    return mCloseAt;
}

void Schedule::setCloseAt(int minuteOfDay)
{
    mCloseAt = minuteOfDay;
}

int Schedule::closeAtHour() const
{
    return mCloseAt / 60;
}

int Schedule::closeAtMinute() const
{
    return mCloseAt % 60;
}

bool Schedule::scheduledForDay(WeekDay weekDay) const
{
    return (mDaysMask & (1 << static_cast<int>(weekDay))) != 0;
}

void Schedule::setScheduledForDay(WeekDay weekDay)
{
    mDaysMask |= (1 << static_cast<int>(weekDay));
}

void Schedule::clearScheduledForDay(WeekDay weekDay)
{
    mDaysMask ^= (1 << static_cast<int>(weekDay));
}

QString Schedule::openAtText() const
{
    QString str;
    float t = openAtUS();
    if(t < 0)
        str.sprintf("%d:%02d %s", -static_cast<int>(t), static_cast<int>(-t*60) % 60, "AM");
    else
        str.sprintf("%d:%02d %s", static_cast<int>(t), static_cast<int>(t*60) % 60, "PM");
    return str;
}

QString Schedule::closeAtText() const
{
    QString str;
    float t = closeAtUS();
    if(t < 0)
        str.sprintf("%d:%02d %s", -static_cast<int>(t), static_cast<int>(-t*60) % 60, "AM");
    else
        str.sprintf("%d:%02d %s", static_cast<int>(t), static_cast<int>(t*60) % 60, "PM");
    return str;
}

float Schedule::openAtUS() const
{
    float f = static_cast<float>(mOpenAt);
    if(f < 60)
        return 12 + f / 60;
    else if(f < 60 * 13)
        return -f / 60;
    else
        return f / 60 - 12;
}

void Schedule::setOpenAtUS(const float openAtUS)
{
    if(openAtUS < 0){
        setOpenAt(static_cast<int>(-openAtUS*60));
    }
    else {
        setOpenAt(static_cast<int>((openAtUS+12) * 60) % 1440);
    }
}

float Schedule::closeAtUS() const
{
    float f = static_cast<float>(mCloseAt);
    if(f < 60)
        return 12 + f / 60;
    else if(f < 60 * 13)
        return -f / 60;
    else
        return f / 60 - 12;
}

void Schedule::setCloseAtUS(const float closeAtUS)
{
    if(closeAtUS < 0){
        setCloseAt(static_cast<int>(-closeAtUS*60));
    }
    else{
        setCloseAt(static_cast<int>((closeAtUS+12)*60) % 1440);
    }
}

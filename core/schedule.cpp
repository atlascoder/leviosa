#include "schedule.h"

Schedule::Schedule():
    mId(-1), mScope(Scope::Controller), mFKey(-1), mDaysMask(0), mOpenAt(8*60), mCloseAt(19*60)
{

}

int Schedule::id() const
{
    return mId;
}

void Schedule::setId(int id)
{
    mId = id;
}

Schedule::Scope Schedule::scope() const
{
    return mScope;
}

void Schedule::setScope(int scope)
{
    mScope = static_cast<Schedule::Scope>(scope);
}

int Schedule::fKey() const
{
    return mFKey;
}

void Schedule::setFKey(int fKey)
{
    mFKey = fKey;
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

#ifndef SCHEDULE_H
#define SCHEDULE_H


class Schedule
{
public:
    Schedule();

    enum class WeekDay { Sun, Mon, Tue, Wed, Thu, Fri, Sat };
    enum class Scope { Controller, ShadesGroup };

    int id() const;
    void setId(int id);

    Scope scope() const;
    void setScope(int scope);

    int fKey() const;
    void setFKey(int fKey);

    int days() const;
    void setDays(int days);

    int openAt() const;
    void setOpenAt(int minuteOfDay);

    int openAtHour() const;
    int openAtMinute() const;

    int closeAt() const;
    void setCloseAt(int minuteOfDay);

    int closeAtHour() const;
    int closeAtMinute() const;

    bool scheduledForDay(WeekDay weekDay) const;
    void setScheduledForDay(WeekDay weekDay);
    void clearScheduledForDay(WeekDay weekDay);

private:
    int mId;
    Scope mScope;
    int mFKey;
    char mDaysMask;
    int mOpenAt;
    int mCloseAt;
};

#endif // SCHEDULE_H

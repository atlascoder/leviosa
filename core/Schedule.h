#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QString>

class Schedule
{
public:
    Schedule();

    enum class WeekDay { Sun, Mon, Tue, Wed, Thu, Fri, Sat };

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

    QString openAtText() const;
    QString closeAtText() const;

    float openAtUS() const;
    void setOpenAtUS(const float openAtUS);

    float closeAtUS() const;
    void setCloseAtUS(const float closeAtUS);

    bool scheduledForDay(WeekDay weekDay) const;
    void setScheduledForDay(WeekDay weekDay);
    void cleanScheduledForDay(WeekDay weekDay);

    void clean();

protected:
    char mDaysMask;
    int mOpenAt;
    int mCloseAt;
};

#endif // SCHEDULE_H

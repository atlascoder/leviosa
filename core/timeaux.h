#ifndef TIMEAUX_H
#define TIMEAUX_H

#include <QString>

namespace timeaux {

static QString utcOffsetToTimezone(int utcOffset)
{
    switch (utcOffset / 3600) {
    case -5:
        return "EST";
    case -6:
        return "CST";
    case -7:
        return "MST";
    case -8:
        return "PST";
    case -9:
        return "AKST";
    case -10:
        return "HAST";
    default:
        if(utcOffset == 0)
            return "UTC";
        else if(utcOffset > 0)
            return "UTC+" + QString::number(utcOffset/3600);
        else
            return "UTC" + QString::number(utcOffset/3600);
    }
}

//static int timezoneToUtcOffset(const QString& timezone)
//{
//    Q_UNUSED(timezone)
//    return 0;
//}

}

#endif // TIMEAUX_H

#ifndef CONTROLLERCFG_H
#define CONTROLLERCFG_H

#include <QString>

class ControllerCfg
{
public:
    ControllerCfg();

    enum State { Undefined, Wan, Searching, Configuring, Wlan, NotFound, Failed};

    QString mMac;
    QString mIp;
    QString mLocationUuid;
    QString mTimezone;
    QString mLastMessage;
    State mState;
};

#endif // CONTROLLERCFG_H

#ifndef USERLOCATION_H
#define USERLOCATION_H

#include <QString>
#include "positioned.h"
#include "Syncable.h"

class UserLocation : public Positioned, public Syncable
{
    QString mBssid;
    QString mSsid;
    QString mName;
    int mUtcOffset;
    bool mIsOnWlan;
    bool mIsOnline;
public:
    explicit UserLocation(const QString &name = "My Home");

    QString bssid() const;
    void setBssid(const QString &bssid);

    QString name() const;
    void setName(const QString &name);

    QString ssid() const;
    void setSsid(const QString &ssid);

    int utcOffset() const;
    void setUtcOffset(int offset);

    bool isOnWlan() const;
    void setIsOnWlan(bool onWlan);

    bool isOnline() const;
    void setOnline(bool isOnline);

    QString syncContent() const override;
    void setSyncContent(const QString & syncContent) override;

};

#endif // USERLOCATION_H

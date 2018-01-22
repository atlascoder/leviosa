#ifndef USERLOCATION_H
#define USERLOCATION_H

#include <QString>

class UserLocation
{
    int mId;
    QString mBssid;
    QString mSsid;
    QString mName;
    int mUtcOffset;
    int mPosition;
    bool mIsLocal;
    bool mIsOnline;
public:
    explicit UserLocation(const QString &name = "My Home");

    int id() const;
    void setId(const int id);

    QString bssid() const;
    void setBssid(const QString &bssid);

    QString name() const;
    void setName(const QString &name);

    QString ssid() const;
    void setSsid(const QString &ssid);

    int utcOffset() const;
    void setUtcOffset(int offset);

    int position() const;
    void setPosition(int position);

    bool isLocal() const;
    void setLocal(bool isLocal);

    bool isOnline() const;
    void setOnline(bool isOnline);
};

#endif // USERLOCATION_H

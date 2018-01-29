#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

class User
{

    QString mEmail;

    QString mIdToken;
    int mIdTokenExpiration;

    QString mAccessToken;
    int mAccessTokenExpiration;

    QString mRefreshToken;
    int mRefreshTokenExpiration;

    int mLocationsModified;
    int mControllersModified;
    int mShadeGroupsModified;

    int mLocationsSyncCount;
    int mControllersSyncCount;
    int mShadeGroupsSyncCount;

    bool mLocationsSynced;
    bool mControllersSynced;
    bool mShadeGroupsSynced;

public:
    User();

    QString email() const;
    void setEmail(const QString &email);

    QString idToken() const;
    void setIdToken(const QString &token);

    int idTokenExpiration() const;
    void setIdTokenExpiration(int time);

    QString accessToken() const;
    void setAccessToken(const QString &token);

    int accessTokenExpiration() const;
    void setAccessTokenExpiration(const int time);

    QString refreshToken() const;
    void setRefreshToken(const QString &token);

    int refreshTokenExpiration() const;
    void setRefreshTokenExpiration(int time);

    int locationsModified() const { return mLocationsModified; }
    void setLocationsModified(int lastModified) { mLocationsModified = lastModified; }

    int controllersModified() const { return mControllersModified; }
    void setControllersModified(int lastModified) { mControllersModified = lastModified; }

    int shadeGroupsLastModified() const { return mShadeGroupsModified; }
    void setShadeGroupsModified(int lastModified) { mShadeGroupsModified = lastModified; }

    int locationsSyncCount() const { return mLocationsSyncCount; }
    void setLocationsSyncCount(int syncCount) { mLocationsSyncCount = syncCount; }

    int controllersSyncCount() const { return mControllersSyncCount; }
    void setControllersSyncCount(int syncCount) { mControllersSyncCount = syncCount; }

    int shadeGroupsSyncCount() const { return mShadeGroupsSyncCount; }
    void setShadeGroupsSyncCount(int syncCount) { mShadeGroupsSyncCount = syncCount; }

    bool locationsSynced() const { return mLocationsSynced; }
    void setLocationsSynced(bool isSynced) { mLocationsSynced = isSynced; }

    bool controllersSynced() const { return mControllersSynced; }
    void setControllersSynced(bool isSynced) { mControllersSynced = isSynced; }

    bool shadeGroupsSynced() const { return mShadeGroupsSynced; }
    void setShadeGroupsSynced(bool isSynced) { mShadeGroupsSynced = isSynced; }
};

#endif // USER_H

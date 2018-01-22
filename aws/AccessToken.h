#ifndef ACCESSTOKEN_H
#define ACCESSTOKEN_H

#include "Jwt.h"

class AccessToken : public Jwt
{
    bool parsePayload(const QJsonObject &payload) override;

    QString mDeviceKey;
    QString mScope;
    QString mJti;
    QString mClientId;
    QString mUsername;
public:
    AccessToken() : Jwt() {}
    AccessToken(const QString & string) : Jwt(string) {}

    QString deviceKey() const;
    QString scope() const;
    QString jti() const;
    QString clientId() const;
    QString username() const;
};

#endif // ACCESSTOKEN_H

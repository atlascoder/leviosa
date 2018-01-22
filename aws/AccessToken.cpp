#include "AccessToken.h"

bool AccessToken::parsePayload(const QJsonObject &payload)
{
    if(!this->Jwt::parsePayload(payload)) return false;

    if(!payload.contains("device_key")) return false;

    mDeviceKey = payload["device_key"].toString();

    if(!payload.contains("scope")) return false;

    mScope = payload["scope"].toString();

    if(!payload.contains("jti")) return false;

    mJti = payload["jti"].toString();

    if(!payload.contains("client_id")) return false;

    mClientId = payload["client_id"].toString();

    if(!payload.contains("username")) return false;

    mUsername = payload["username"].toString();


    return true;
}

QString AccessToken::deviceKey() const
{
    return mDeviceKey;
}

QString AccessToken::scope() const
{
    return mScope;
}

QString AccessToken::jti() const
{
    return mJti;
}

QString AccessToken::clientId() const
{
    return mClientId;
}

QString AccessToken::username() const
{
    return mUsername;
}

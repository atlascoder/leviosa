#include "IdToken.h"

bool IdToken::parsePayload(const QJsonObject &payload)
{
    if(!this->Jwt::parsePayload(payload)) return false;

    if(!payload.contains("cognito:username")) return false;
    mUsername = payload["cognito:username"].toString();

    if(!payload.contains("aud")) return false;
    mAud = payload["aud"].toString();

    if(!payload.contains("auth_time") || !payload["auth_time"].isDouble()) return false;
    mAuthTime = payload["auth_time"].toInt();

    if(!payload.contains("email")) return false;
    mEmail = payload["email"].toString();

    if(!payload.contains("email_verified") || !payload["email_verified"].isBool()) return false;
    mEmailVerified = payload["email_verified"].toBool();

    return true;
}

QString IdToken::username() const
{
    return mUsername;
}

QString IdToken::aud() const
{
    return mAud;
}

int IdToken::authTime() const
{
    return mAuthTime;
}

QString IdToken::email() const
{
    return mEmail;
}

bool IdToken::emailVerified() const
{
    return mEmailVerified;
}

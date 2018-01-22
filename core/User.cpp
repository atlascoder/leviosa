#include "User.h"

User::User():
    mEmail(),
    mIdToken(), mIdTokenExpiration(0),
    mAccessToken(), mAccessTokenExpiration(0),
    mRefreshToken(), mRefreshTokenExpiration(0)
{

}

QString User::email() const
{
    return mEmail;
}

void User::setEmail(const QString &email)
{
    mEmail = email;
}

QString User::idToken() const
{
    return mIdToken;
}

void User::setIdToken(const QString &token)
{
    mIdToken = token;
}

int User::idTokenExpiration() const
{
    return mIdTokenExpiration;
}

void User::setIdTokenExpiration(int time)
{
    mIdTokenExpiration = time;
}

QString User::accessToken() const
{
    return mAccessToken;
}

void User::setAccessToken(const QString &token)
{
    mAccessToken = token;
}

int User::accessTokenExpiration() const
{
    return mAccessTokenExpiration;
}

void User::setAccessTokenExpiration(int time)
{
    mAccessTokenExpiration = time;
}

QString User::refreshToken() const
{
    return mRefreshToken;
}

void User::setRefreshToken(const QString &token)
{
    mRefreshToken = token;
}

int User::refreshTokenExpiration() const
{
    return mRefreshTokenExpiration;
}

void User::setRefreshTokenExpiration(int time)
{
    mRefreshTokenExpiration = time;
}

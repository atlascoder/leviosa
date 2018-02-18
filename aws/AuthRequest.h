#ifndef AUTHREQUEST_H
#define AUTHREQUEST_H

#include <QString>
#include <QByteArray>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-idp/CognitoIdentityProviderClient.h>
#include <boost/multiprecision/cpp_int.hpp>
#include "RefreshToken.h"


class AuthRequest
{
    QString mEmail;
    QString mPassword;
    bool mSuccessful;
    bool mConfirmed;
    QString mIdToken;
    QString mAccessToken;
    QString mRefreshToken;
    QString mLastMessage;
    int mRefreshTokenExpiration;

    boost::multiprecision::cpp_int mN;
    boost::multiprecision::cpp_int mg;
    boost::multiprecision::cpp_int mk;
    boost::multiprecision::cpp_int ma;
    boost::multiprecision::cpp_int mA;

    bool mCancelled;

    Aws::CognitoIdentityProvider::CognitoIdentityProviderClient* mClient;

public:
    AuthRequest();
    ~AuthRequest();

    void signUp(const QString& email, const QString& password);
    void verify(const QString& email, const QString& code);
    void requestConfirmation(const QString& email);
    void signIn(const QString& email, const QString& password);
    void signOut();
    void restorePassword(const QString& email);
    void changePassword(const QString& email, const QString& newPassword, const QString& password);

    QString getIdToken() const { return mIdToken; }
    QString getAccessToken() const { return mAccessToken; }
    QString getRefreshToken() const { return mRefreshToken; }
    bool isSuccessful() const { return mSuccessful; }
    QString getLastMessage() const { return mLastMessage; }
    int getRefreshTokenExpiration() const { return mRefreshTokenExpiration; }

    bool refreshTokens(const QString& email, const QString&  refreshToken, int refreshTokenExpiration);

    bool userConfirmed() const {return mConfirmed; }

    void cancelRequests();
private:

    QByteArray authenticateUser(const QString &userId, const QString & password, const QString &saltString, const QString &srp_b, const QString &secret_block, const QString &formattedTimestamp);
    QByteArray getPasswordAuthenticationKey(const QString &userId, const QString &password, const QByteArray &A, const QByteArray &B, const QString &salt);

};

#endif // AUTHREQUEST_H

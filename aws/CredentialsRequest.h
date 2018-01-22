#ifndef CREDENTIALSREQUEST_H
#define CREDENTIALSREQUEST_H

#include <QString>
#include <aws/cognito-identity/model/Credentials.h>
#include <aws/cognito-identity/CognitoIdentityErrors.h>
#include "IdToken.h"
#include "RefreshToken.h"

class CredentialsRequest
{
    IdToken mIdToken;
    bool mIsSuccessful;
    Aws::CognitoIdentity::CognitoIdentityErrors mError;
    QString mLastMessage;
    Aws::CognitoIdentity::Model::Credentials mCredentials;
public:
    CredentialsRequest(const IdToken & idToken) : mIdToken(idToken), mIsSuccessful(false) {}

    bool isSuccessful() const { return mIsSuccessful; }
    Aws::CognitoIdentity::CognitoIdentityErrors getError() const { return mError; }
    QString getLastMessage() const { return mLastMessage; }
    Aws::CognitoIdentity::Model::Credentials getCredentials() const { return mCredentials; }

    void request();

};

#endif // CREDENTIALSREQUEST_H

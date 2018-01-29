#ifndef CREDENTIALSREQUEST_H
#define CREDENTIALSREQUEST_H

#include <QString>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-identity/model/Credentials.h>
#include <aws/cognito-identity/CognitoIdentityErrors.h>
#include "IdToken.h"
#include "RefreshToken.h"
#include "ClientConfig.h"

class CredentialsRequest
{
    IdToken mIdToken;
    bool mIsSuccessful;
    Aws::CognitoIdentity::CognitoIdentityErrors mError;
    QString mLastMessage;
    Aws::CognitoIdentity::Model::Credentials mCredentials;
    bool mCancelled;

    Aws::CognitoIdentity::CognitoIdentityClient* mClient;
public:
    CredentialsRequest();
    ~CredentialsRequest();

    void setIdToken(const IdToken& idToken);

    bool isSuccessful() const { return mIsSuccessful; }
    Aws::CognitoIdentity::CognitoIdentityErrors getError() const { return mError; }
    QString getLastMessage() const { return mLastMessage; }
    Aws::CognitoIdentity::Model::Credentials getCredentials() const { return mCredentials; }

    void requestId();
    void requestCredentials();

    void cancelRequests();

};

#endif // CREDENTIALSREQUEST_H

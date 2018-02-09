#include "CredentialsRequest.h"
#include <QStringList>

#include <aws/cognito-identity/CognitoIdentityRequest.h>
#include <aws/core/utils/Outcome.h>

#include <aws/cognito-identity/model/GetCredentialsForIdentityRequest.h>
#include <aws/cognito-identity/model/GetCredentialsForIdentityResult.h>

#include <aws/cognito-identity/model/GetIdRequest.h>
#include <aws/cognito-identity/model/GetIdResult.h>
#include <aws/core/utils/memory/AWSMemory.h>

#include "ClientConfig.h"

#include <QDebug>

CredentialsRequest::CredentialsRequest() : mIdToken(), mIsSuccessful(false), mCancelled(true)
{
    mClient = Aws::New<Aws::CognitoIdentity::CognitoIdentityClient>(nullptr, ClientConfig::instance());
}

CredentialsRequest::~CredentialsRequest()
{
    cancelRequests();
    Aws::Delete<Aws::CognitoIdentity::CognitoIdentityClient>(mClient);
}

void CredentialsRequest::setIdToken(const IdToken &idToken)
{
    mIdToken = idToken;
}

void CredentialsRequest::cancelRequests()
{
    qDebug() << "Stop credentials requests";
    if(mCancelled) return;
    mClient->DisableRequestProcessing();
    mCancelled = true;
}

void CredentialsRequest::requestId()
{
    mCancelled = false;
    Aws::CognitoIdentity::Model::GetIdRequest idReq;
    idReq.SetAccountId(ClientConfig::instance().accoundId);
    idReq.SetIdentityPoolId(ClientConfig::instance().identityPool);
    QString login = mIdToken.issuer().split("//").at(1);
    idReq.AddLogins(Aws::Utils::StringUtils::to_string(login.toStdString()), Aws::Utils::StringUtils::to_string(mIdToken.raw().toStdString()));

    if(mCancelled) return;

    auto idResp = mClient->GetId(idReq);
    if(idResp.IsSuccess()){
        qDebug() << "Identity ID: " << idResp.GetResult().GetIdentityId().c_str() << endl;
        ClientConfig::instance().identityId = idResp.GetResult().GetIdentityId();
        mIsSuccessful = true;
    }
    else {
        mLastMessage = QString(idResp.GetError().GetMessage().c_str());
        mError = idResp.GetError().GetErrorType();
        mIsSuccessful = false;
    }
    mCancelled = true;
}

void CredentialsRequest::requestCredentials()
{
    mCancelled = false;
    Aws::CognitoIdentity::Model::GetCredentialsForIdentityRequest req;
    req.SetIdentityId(ClientConfig::instance().identityId);
    QString login = mIdToken.issuer().split("//").at(1);
    req.AddLogins(Aws::Utils::StringUtils::to_string(login.toStdString()), Aws::Utils::StringUtils::to_string(mIdToken.raw().toStdString()));
    if(mCancelled) return;
    auto creds = mClient->GetCredentialsForIdentity(req);
    if(creds.IsSuccess()){
        qDebug() << "Access Key: " << creds.GetResult().GetCredentials().GetAccessKeyId().c_str() << " => " << creds.GetResult().GetCredentials().GetSecretKey().c_str() << endl;
        mCredentials = creds.GetResult().GetCredentials();
        mIsSuccessful = true;
    }
    else{
        mLastMessage = QString(creds.GetError().GetMessage().c_str());
        mError = creds.GetError().GetErrorType();
    }
    mCancelled = true;
}

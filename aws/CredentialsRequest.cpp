#include "CredentialsRequest.h"
#include <QStringList>

#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-identity/CognitoIdentityRequest.h>
#include <aws/core/utils/Outcome.h>

#include <aws/cognito-identity/model/GetCredentialsForIdentityRequest.h>
#include <aws/cognito-identity/model/GetCredentialsForIdentityResult.h>

#include <aws/cognito-identity/model/GetIdRequest.h>
#include <aws/cognito-identity/model/GetIdResult.h>

#include "ClientConfig.h"

#include <QDebug>

void CredentialsRequest::request()
{
    Aws::CognitoIdentity::CognitoIdentityClient cl(ClientConfig::instance());
    Aws::CognitoIdentity::Model::GetIdRequest idReq;
    idReq.SetAccountId(ClientConfig::instance().accoundId);
    idReq.SetIdentityPoolId(ClientConfig::instance().identityPool);
    QString login = mIdToken.issuer().split("//").at(1);
    idReq.AddLogins(login.toStdString(), mIdToken.raw().toStdString());

    auto idResp = cl.GetId(idReq);

    if(idResp.IsSuccess()){
        qDebug() << "Identity ID: " << idResp.GetResult().GetIdentityId().c_str() << endl;
        ClientConfig::instance().identityId = idResp.GetResult().GetIdentityId();
        Aws::CognitoIdentity::Model::GetCredentialsForIdentityRequest req;
        req.SetIdentityId(idResp.GetResult().GetIdentityId());
        req.AddLogins(login.toStdString(), mIdToken.raw().toStdString());
        auto creds = cl.GetCredentialsForIdentity(req);
        if(creds.IsSuccess()){
            qDebug() << "Access Key: " << creds.GetResult().GetCredentials().GetAccessKeyId().c_str() << " => " << creds.GetResult().GetCredentials().GetSecretKey().c_str() << endl;
            mCredentials = creds.GetResult().GetCredentials();
            mIsSuccessful = true;
        }
        else{
            mLastMessage = QString(creds.GetError().GetMessage().c_str());
            mError = creds.GetError().GetErrorType();
        }
    }
    else{
        mLastMessage = QString(idResp.GetError().GetMessage().c_str());
        mError = idResp.GetError().GetErrorType();
    }
}

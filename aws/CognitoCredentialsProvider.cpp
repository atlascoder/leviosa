#include "CognitoCredentialsProvider.h"

#include <QDebug>
#include <QDateTime>

#include <aws/core/utils/DateTime.h>
#include <aws/cognito-idp/model/InitiateAuthRequest.h>
#include <aws/cognito-idp/model/InitiateAuthResult.h>
#include <aws/identity-management/auth/CognitoCachingCredentialsProvider.h>
#include <aws/identity-management/auth/PersistentCognitoIdentityProvider.h>
#include <aws/cognito-identity/model/GetCredentialsForIdentityRequest.h>
#include <aws/cognito-identity/model/GetIdRequest.h>
#include <aws/core/utils/Outcome.h>

#include <QMutexLocker>

#include "Jwt.h"
#include "CurrentUser.h"

CognitoCredentialsProvider::CognitoCredentialsProvider(
        const std::shared_ptr<PersistentCredentialsProviderSQLite> &identityRepository,
        const std::shared_ptr<CognitoIdentity::CognitoIdentityClient> &cognitoIdentityClient,
        const std::shared_ptr<Aws::CognitoIdentityProvider::CognitoIdentityProviderClient> &idpClient):
    Aws::Auth::CognitoCachingAuthenticatedCredentialsProvider(identityRepository, cognitoIdentityClient),
    mIdpClient(idpClient)
{ }


bool CognitoCredentialsProvider::RefreshTokens(Auth::LoginAccessTokens& login)
{
    String refreshToken = login.longTermToken;
    long long refreshTokenExpiration = login.longTermTokenExpiry;
    long long now = QDateTime::currentSecsSinceEpoch();
    if (refreshTokenExpiration== 0) {
        return false;
    }
    else if(refreshTokenExpiration > now - 300){
        String email(CurrentUser::instance().email().toStdString());
        Aws::CognitoIdentityProvider::Model::InitiateAuthRequest initAuthReq;
        initAuthReq.SetAuthFlow(Aws::CognitoIdentityProvider::Model::AuthFlowType::REFRESH_TOKEN_AUTH);
        initAuthReq.SetClientId(AWS_CLIENT_ID);
        initAuthReq.AddAuthParameters("USERNAME", email);
        initAuthReq.AddAuthParameters("REFRESH_TOKEN", refreshToken);
        Aws::CognitoIdentityProvider::Model::InitiateAuthOutcome initAuthResp = mIdpClient->InitiateAuth(initAuthReq);
        if(initAuthResp.IsSuccess()){
            login.accessToken = initAuthResp.GetResult().GetAuthenticationResult().GetIdToken();
            return true;
        }
        else {
            qDebug() << "Error while refreshing tokens: " << initAuthResp.GetError().GetMessage().c_str();
        }
    }
    return false;
}

Aws::Auth::AWSCredentials CognitoCredentialsProvider::GetAWSCredentials()
{
    if (m_identityRepository->HasLogins()) {
        Map<String, Aws::Auth::LoginAccessTokens> logins = m_identityRepository->GetLogins();
        bool updated = false;
        for (auto& l : logins) {
            Jwt token(l.second.accessToken.c_str());
            if (token.expiration() < Aws::Utils::DateTime::Now().SecondsWithMSPrecision() - 120) {
                QMutexLocker locker(&mRefreshTokensMutex);
                token.setRaw(l.second.accessToken.c_str());
                if (token.expiration() < Aws::Utils::DateTime::Now().SecondsWithMSPrecision() - 120) {
                    if (RefreshTokens(l.second))
                        updated = true;
                }
            }
        }
        if (updated) {
            m_identityRepository->PersistLogins(logins);
        }
    }

    return Auth::CognitoCachingAuthenticatedCredentialsProvider::GetAWSCredentials();
}

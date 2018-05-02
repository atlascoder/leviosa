#ifndef COGNITOCREDENTIALSPROVIDER_H
#define COGNITOCREDENTIALSPROVIDER_H

#include <aws/identity-management/auth/CognitoCachingCredentialsProvider.h>
#include <aws/cognito-idp/CognitoIdentityProviderClient.h>
#include <aws/core/utils/Outcome.h>

#include <QMutex>

#include "PersistentCredentialsProviderSQL.h"
#include "ClientConfig.h"

using namespace Aws;


class CognitoCredentialsProvider : public Auth::CognitoCachingAuthenticatedCredentialsProvider
{
    QMutex mRefreshTokensMutex;
    QMutex mCredsMutex;

    std::shared_ptr<CognitoIdentityProvider::CognitoIdentityProviderClient> mIdpClient;
public:
    CognitoCredentialsProvider(
            const std::shared_ptr<PersistentCredentialsProviderSQLite>& identityRepository,
            const std::shared_ptr<CognitoIdentity::CognitoIdentityClient>& cognitoIdentityClient,
            const std::shared_ptr<CognitoIdentityProvider::CognitoIdentityProviderClient>& idpClient
        );
    virtual Auth::AWSCredentials GetAWSCredentials() override;
private:
    bool RefreshTokens(Auth::LoginAccessTokens& logins);
};

#endif // COGNITOCREDENTIALSPROVIDER_H

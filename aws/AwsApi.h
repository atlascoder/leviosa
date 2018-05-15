#ifndef AWSAPI_H
#define AWSAPI_H

#include <aws/identity-management/auth/CognitoCachingCredentialsProvider.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/core/client/AWSClient.h>

#include <aws/cognito-sync/CognitoSyncClient.h>
#include <aws/cognito-idp/CognitoIdentityProviderClient.h>
#include <aws/iot/IoTClient.h>

#include "PersistentCredentialsProviderSQL.h"
#include "CognitoCredentialsProvider.h"
#include "ClientConfig.h"

using namespace std;
using namespace Aws;

class AwsApi
{
    unique_ptr<vector<shared_ptr<Client::AWSClient>>> mClients;
    bool mCancelled;
    shared_ptr<PersistentCredentialsProviderSQLite> mPersistentCredentialsProvider;
    shared_ptr<CognitoIdentity::CognitoIdentityClient> mIdentityClient;
    shared_ptr<Aws::CognitoIdentityProvider::CognitoIdentityProviderClient> mIdpClient;
    shared_ptr<CognitoCredentialsProvider> mCredentialsProvider;
    mutex mClearMutex;
public:
    static AwsApi& instance() {
        static AwsApi sInstance;
        return sInstance;
    }

    void reset();

    void stopRequests();

    void recycleClient(const shared_ptr<Client::AWSClient>& client);

    void persistLogins(const Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens>& logins) {
        mPersistentCredentialsProvider->PersistLogins(logins);
    }

    Aws::String getIdentityId();
    Aws::Auth::AWSCredentials getAwsCredentials();

    bool HasLogins();

    template <class T>
    shared_ptr<T> getClient() {
        if (mCancelled) return make_shared<T>(nullptr);
        shared_ptr<T> client = Aws::MakeShared<T>(nullptr, mCredentialsProvider, ClientConfig::instance());
        mClients->push_back(client);
        return client;
    }
private:
    AwsApi();
    ~AwsApi();

    void clearClients();
};

#endif // AWSAPI_H

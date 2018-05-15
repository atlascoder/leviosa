#include "AwsApi.h"
#include <QMutexLocker>

using namespace std;

AwsApi::AwsApi(): mClients(new vector<shared_ptr<Client::AWSClient>>), mCancelled(false)
{
    reset();
}

AwsApi::~AwsApi()
{
    stopRequests();
}

void AwsApi::reset()
{
    mPersistentCredentialsProvider = MakeShared<PersistentCredentialsProviderSQLite>(
                nullptr,
                AWS_IDENTITY_POOL,
                AWS_ACCOUNT_ID
            );
    mIdentityClient = MakeShared<CognitoIdentity::CognitoIdentityClient>(
                nullptr,
                ClientConfig::instance()
            );
    mIdpClient = MakeShared<Aws::CognitoIdentityProvider::CognitoIdentityProviderClient>(
                nullptr,
                ClientConfig::instance()
            );
    mCredentialsProvider = MakeShared<CognitoCredentialsProvider>(
                nullptr,
                mPersistentCredentialsProvider,
                mIdentityClient,
                mIdpClient
            );
}

void AwsApi::stopRequests()
{
    mCancelled = true;
    mIdentityClient->DisableRequestProcessing();
    mIdpClient->DisableRequestProcessing();
    clearClients();
}

void AwsApi::recycleClient(const shared_ptr<Client::AWSClient>& client)
{
    lock_guard<mutex> guard(mClearMutex);
    vector<shared_ptr<Client::AWSClient>>::iterator c = find_if(mClients->begin(), mClients->end(), [&client](const shared_ptr<Client::AWSClient>& cli)->bool{ return cli.get()==client.get(); });
    if (c != mClients->end()) {
        c->get()->DisableRequestProcessing();
        mClients->erase(c);
    }
}

void AwsApi::clearClients()
{
    while (mClients->size() > 0) {
        recycleClient(mClients->back());
    }
}

Aws::String AwsApi::getIdentityId()
{
    if (!mPersistentCredentialsProvider->HasIdentityId()) {
        if (mCancelled) return "";
        mCredentialsProvider->GetAWSCredentials();
    }
    if (mCancelled) return "";
    return mPersistentCredentialsProvider->GetIdentityId();
}

Aws::Auth::AWSCredentials AwsApi::getAwsCredentials()
{
    if (mCancelled) return Auth::AWSCredentials();
    return mCredentialsProvider->GetAWSCredentials();
}

bool AwsApi::HasLogins()
{
    return mPersistentCredentialsProvider->HasLogins();
}

#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H

#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>

#define AWS_ACCOUNT_ID "922040780541"
#define AWS_POOL_NAME "yUCD5B4i3"
#define AWS_CLIENT_ID "7pnqfuge1eqk612hn5jq4r9dta"
#define AWS_REGION "us-east-2"
#define AWS_IDENTITY_POOL "us-east-2:7b20c740-8cb4-4c12-b736-8920e967b102"

class ClientConfig : public Aws::Client::ClientConfiguration
{

public:
    static ClientConfig& instance();

    Aws::String userPoolId;
    Aws::String userPoolName;
    Aws::String clientId;
    Aws::String identityPool;
    Aws::String accoundId;
    Aws::String identityId;
private:
    ClientConfig() : ClientConfiguration()
    {
        region = AWS_REGION;
        scheme = Aws::Http::Scheme::HTTPS;
        connectTimeoutMs = 40000;
        requestTimeoutMs = 60000;
        clientId = AWS_CLIENT_ID;
        userPoolName = AWS_POOL_NAME;
        userPoolId = region + "_" + userPoolName;
        identityPool = AWS_IDENTITY_POOL;
        accoundId = AWS_ACCOUNT_ID;
        verifySSL = false;
    }
};

#endif // CLIENTCONFIG_H

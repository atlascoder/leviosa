#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H

#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>

#define AWS_ACCOUNT_ID "559850651912"
#define AWS_POOL_NAME "gtQc8ZadV"
#define AWS_CLIENT_ID "5toerkjrgrp3j35c5n9f7f8kv"
#define AWS_REGION "us-east-2"
#define AWS_IDENTITY_POOL "us-east-2:39c0e98d-b455-4dc4-b3ea-847ba0968afe"

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

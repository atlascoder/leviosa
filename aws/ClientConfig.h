#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H

#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/core/client/DefaultRetryStrategy.h>
#include <QtDebug>

#ifdef QT_DEBUG
#define AWS_ACCOUNT_ID "559850651912"
#define AWS_POOL_NAME "gtQc8ZadV"
#define AWS_CLIENT_ID "5toerkjrgrp3j35c5n9f7f8kv"
#define AWS_REGION "us-east-2"
#define AWS_IDENTITY_POOL "us-east-2:39c0e98d-b455-4dc4-b3ea-847ba0968afe"
#else
#define AWS_ACCOUNT_ID "922040780541"
#define AWS_POOL_NAME "y40XQdASz"
#define AWS_CLIENT_ID "2hgvpdva1dfa07s4e6k4i4n888"
#define AWS_REGION "us-east-2"
#define AWS_IDENTITY_POOL "us-east-2:7b20c740-8cb4-4c12-b736-8920e967b102"
#endif


class ClientConfig : public Aws::Client::ClientConfiguration
{

public:
    static ClientConfig& instance();
    ClientConfig();

    Aws::String userPoolId;
    Aws::String identityPool;
    Aws::String clientId;
    Aws::String userPoolName;
    Aws::String accountId;
    Aws::String loginPoint;
};

#endif // CLIENTCONFIG_H

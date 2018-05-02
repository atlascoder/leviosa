#include <aws/core/platform/OSVersionInfo.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/threading/Executor.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/Version.h>


#include "ClientConfig.h"
#include "qsystemdetection.h"

#if defined(Q_OS_ANDROID)
namespace Aws {
    namespace OSVersionInfo {
        Aws::String ComputeOSVersionString()
        {
            return " Android OS";
        }
    }
}
#endif

ClientConfig& ClientConfig::instance()
{
    static ClientConfig singleton;
    return singleton;
}

ClientConfig::ClientConfig()
{
    region = AWS_REGION;
    scheme = Aws::Http::Scheme::HTTPS;
    connectTimeoutMs = 40000;
    requestTimeoutMs = 60000;
    clientId = AWS_CLIENT_ID;
    userPoolName = AWS_POOL_NAME;
    userPoolId = region + "_" + userPoolName;
    identityPool = AWS_IDENTITY_POOL;
    accountId = AWS_ACCOUNT_ID;
    loginPoint = "cognito-idp." + region + ".amazonaws.com/" + userPoolId;
#if defined (Q_OS_ANDROID)
    verifySSL = false;
#endif
}

#include "aws/CognitoSyncCommand.h"
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-sync/model/Record.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/cognito-sync/model/ListRecordsRequest.h>
#include <aws/cognito-sync/model/ListRecordsResult.h>
#include <aws/cognito-sync/CognitoSyncRequest.h>
#include <aws/core/utils/Outcome.h>
#include <aws/cognito-sync/model/UpdateRecordsRequest.h>
#include <aws/cognito-sync/model/UpdateRecordsResult.h>
#include <aws/cognito-sync/model/RecordPatch.h>
#include <aws/core/utils/memory/AWSMemory.h>

#include <QStringBuilder>
#include <QDebug>
#include <QMutexLocker>
#include <memory>

#include "AwsApi.h"
#include "CurrentUser.h"
#include "ClientConfig.h"

using namespace Aws;
using namespace std;

void CognitoSyncCommand::sendCommand(const QString &controllerMac, const QString& command, int channel)
{
    qDebug() << "Command started";
    mIsSuccessful = false;
    shared_ptr<CognitoSync::CognitoSyncClient> mClient = AwsApi::instance().getClient<CognitoSync::CognitoSyncClient>();
    CognitoSync::Model::ListRecordsRequest req;
    req.SetDatasetName("commands");
    req.SetIdentityId(AwsApi::instance().getIdentityId());
    req.SetIdentityPoolId(ClientConfig::instance().identityPool);

    auto resp = mClient->ListRecords(req);

    if(resp.IsSuccess()){
        qDebug() << "SYNC Commands received: " << resp.GetResult().GetCount();

        int syncCount = 0;
        for(auto& record: resp.GetResult().GetRecords()){
            if("command" == record.GetKey()){
                syncCount = record.GetSyncCount();
                break;
            }
        }

        CognitoSync::Model::UpdateRecordsRequest updReq;
        updReq.SetDatasetName("commands");
        updReq.SetSyncSessionToken(resp.GetResult().GetSyncSessionToken());
        updReq.SetIdentityId(AwsApi::instance().getIdentityId());
        updReq.SetIdentityPoolId(ClientConfig::instance().identityPool);
        CognitoSync::Model::RecordPatch cmd;
        cmd.SetSyncCount(syncCount);
        cmd.SetOp(CognitoSync::Model::Operation::replace);
        QString cmdLine = controllerMac % " " % command % " " % QString::number(channel);
        cmd.SetValue(Utils::StringUtils::to_string(cmdLine.toStdString()));
        cmd.SetKey("command");
        updReq.AddRecordPatches(cmd);
        auto uResp = mClient->UpdateRecords(updReq);
        if(uResp.IsSuccess()){
            mIsSuccessful = true;
            qDebug() << "Command has been sent: " << cmdLine;
        }
        else{
            qDebug() << "Command failed: " << cmdLine << " with error " << uResp.GetError().GetMessage().c_str();
        }
    }
    else{
        qDebug() << "Command preparing failed: " << " with error " << resp.GetError().GetMessage().c_str();
    }
    AwsApi::instance().recycleClient(mClient);
}

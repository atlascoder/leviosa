#include "CognitoSyncCommand.h"
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

#include <QDebug>

#include "ClientConfig.h"
#include "CurrentUser.h"
#include <memory>

using namespace Aws::CognitoSync;
using namespace std;

CognitoSyncCommand::CognitoSyncCommand(QObject *parent) : QObject(parent),  mCancelled(false)
{
    mClient = Aws::New<Aws::CognitoSync::CognitoSyncClient>(nullptr, ClientConfig::instance());
}

CognitoSyncCommand::~CognitoSyncCommand()
{
    cancelRequests();
    Aws::Delete<Aws::CognitoSync::CognitoSyncClient>(mClient);
}

void CognitoSyncCommand::resetWithCredentials(const Aws::Auth::AWSCredentials &credentials)
{
    cancelRequests();
    Aws::Delete<Aws::CognitoSync::CognitoSyncClient>(mClient);
    mClient = Aws::New<Aws::CognitoSync::CognitoSyncClient>(nullptr, credentials, ClientConfig::instance());
    mCancelled = false;
}

void CognitoSyncCommand::cancelRequests()
{
    mCancelled = true;
    mClient->DisableRequestProcessing();
}

void CognitoSyncCommand::sendCommand(const QString &controllerMac, const QString& command, int channel)
{
    CurrentUser& u = CurrentUser::instance();
    if(mCancelled) return;
    u.requestCredentials();

    if(u.isAuthenticated() && u.hasCredentials()){
        Aws::Auth::AWSCredentials creds;
        u.fillCredentials(creds);
        resetWithCredentials(creds);
    }
    else{
        qDebug() << "Can't get credentials for request";
        emit commandFailed();
        return;
    }

    Model::ListRecordsRequest req;
    req.SetDatasetName("commands");
    req.SetIdentityId(ClientConfig::instance().identityId);
    req.SetIdentityPoolId(ClientConfig::instance().identityPool);

    if(mCancelled) return;
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

        Model::UpdateRecordsRequest updReq;
        updReq.SetDatasetName("commands");
        updReq.SetSyncSessionToken(resp.GetResult().GetSyncSessionToken());
        updReq.SetIdentityId(ClientConfig::instance().identityId);
        updReq.SetIdentityPoolId(ClientConfig::instance().identityPool);
        Model::RecordPatch cmd;
        cmd.SetSyncCount(syncCount);
        cmd.SetOp(Model::Operation::replace);
        QString cmdLine = QString(controllerMac).append(" ").append(command).append(" ").append(QString::number(channel));
        cmd.SetValue(Aws::Utils::StringUtils::to_string(cmdLine.toStdString()));
        cmd.SetKey("command");
        updReq.AddRecordPatches(cmd);
        if(mCancelled) return;
        auto uResp = mClient->UpdateRecords(updReq);
        if(uResp.IsSuccess()){
            emit commandSent();
            qDebug() << "Command has been sent: " << cmdLine;
        }
        else{
            emit commandFailed();
            qDebug() << "Command failed: " << cmdLine << " with error " << uResp.GetError().GetMessage().c_str();
        }
    }
    else{
        emit commandFailed();
        qDebug() << "Command preparing failed: " << " with error " << resp.GetError().GetMessage().c_str();
    }
}

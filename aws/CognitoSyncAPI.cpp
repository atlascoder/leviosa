#include "CognitoSyncAPI.h"
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/cognito-sync/model/ListRecordsRequest.h>
#include <aws/cognito-sync/model/ListRecordsResult.h>
#include <aws/cognito-sync/CognitoSyncRequest.h>
#include <aws/core/utils/Outcome.h>
#include <aws/cognito-sync/model/UpdateRecordsResult.h>
#include <aws/cognito-sync/model/RecordPatch.h>
#include <aws/core/utils/memory/AWSMemory.h>

#include <QDebug>

#include "ClientConfig.h"
#include "core/SyncableRecord.h"
#include <memory>

using namespace Aws::CognitoSync;
using namespace std;

CognitoSyncAPI::CognitoSyncAPI() : mCancelled(false)
{
    mClient = Aws::New<Aws::CognitoSync::CognitoSyncClient>(nullptr, ClientConfig::instance());
}

CognitoSyncAPI::~CognitoSyncAPI()
{
    cancelRequests();
    Aws::Delete<Aws::CognitoSync::CognitoSyncClient>(mClient);
}

void CognitoSyncAPI::resetWithCredentials(const Aws::Auth::AWSCredentials &credentials)
{
    Aws::Delete<Aws::CognitoSync::CognitoSyncClient>(mClient);
    mClient = Aws::New<Aws::CognitoSync::CognitoSyncClient>(nullptr, credentials, ClientConfig::instance());
}

void CognitoSyncAPI::cancelRequests()
{
    mCancelled = true;
    mClient->DisableRequestProcessing();
}

void CognitoSyncAPI::sync(const Aws::Auth::AWSCredentials & credentials, const QString & datasetName, SyncableRecord<Location> & locations, SyncableRecord<Controller> & controllers, SyncableRecord<ShadeGroup> & shadeGroups)
{
    if(mCancelled) return;
    resetWithCredentials(credentials);
    Model::ListRecordsRequest req;
    req.SetDatasetName(Aws::Utils::StringUtils::to_string(datasetName.toStdString()));
    req.SetIdentityId(ClientConfig::instance().identityId);
    req.SetIdentityPoolId(ClientConfig::instance().identityPool);
    Model::UpdateRecordsRequest updateRequest;

    bool locationsFound = false;
    bool controllersFound = false;
    bool shadeGroupsFound = false;

    // receive remote data and mark updated by lastModifiedDate
    if(mCancelled) return;
    auto resp = mClient->ListRecords(req);
    if(resp.IsSuccess()){
        qDebug() << "SYNC ListRecords (" << resp.GetResult().GetCount() << ")";
        const Aws::Vector<Model::Record> & records = resp.GetResult().GetRecords();
        for(auto& record : records){
            qDebug() << "SYNC Record [" << record.GetKey().c_str() << "][" << record.GetSyncCount() << "] => " << record.GetValue().c_str();
            if("locations" == record.GetKey()){
                syncRecord(locations, record, updateRequest);
                locationsFound = true;
            }
            else if("controllers" == record.GetKey()){
                syncRecord(controllers, record, updateRequest);
                controllersFound = true;
            }
            else if("shadeGroups" == record.GetKey()){
                syncRecord(shadeGroups, record, updateRequest);
                shadeGroupsFound = true;
            }
        }
    }
    else{
        qDebug() << "Error receiving Locations: " << resp.GetError().GetMessage().c_str();
        return;
    }

    if(!locationsFound){
        Model::RecordPatch patch;
        patch.SetOp(Model::Operation::replace);
        patch.SetKey("locations");
        patch.SetValue(Aws::Utils::StringUtils::to_string(locations.syncContent().toStdString()));
        patch.SetSyncCount(0);
        updateRequest.AddRecordPatches(patch);
        qDebug() << "SYNC PUT [locations] => " << patch.GetValue().c_str();
    }

    if(!controllersFound){
        Model::RecordPatch patch;
        patch.SetOp(Model::Operation::replace);
        patch.SetKey("controllers");
        patch.SetValue(Aws::Utils::StringUtils::to_string(controllers.syncContent().toStdString()));
        patch.SetSyncCount(0);
        updateRequest.AddRecordPatches(patch);
        qDebug() << "SYNC PUT [locations] => " << patch.GetValue().c_str();
    }

    if(!shadeGroupsFound){
        Model::RecordPatch patch;
        patch.SetOp(Model::Operation::replace);
        patch.SetKey("shadeGroups");
        patch.SetValue(Aws::Utils::StringUtils::to_string(shadeGroups.syncContent().toStdString()));
        patch.SetSyncCount(0);
        updateRequest.AddRecordPatches(patch);
        qDebug() << "SYNC PUT [locations] => " << patch.GetValue().c_str();
    }


    if(updateRequest.GetRecordPatches().size() == 0) return;

    updateRequest.SetIdentityId(ClientConfig::instance().identityId);
    updateRequest.SetIdentityPoolId(ClientConfig::instance().identityPool);
    updateRequest.SetDatasetName(Aws::Utils::StringUtils::to_string(datasetName.toStdString()));
    updateRequest.SetSyncSessionToken(resp.GetResult().GetSyncSessionToken());

    if(mCancelled) return;
    auto updateResp = mClient->UpdateRecords(updateRequest);
    if(updateResp.IsSuccess()){
        const Aws::Vector<Model::Record> & records = updateResp.GetResult().GetRecords();
        for(auto& record : records){
            if("locations" == record.GetKey()){
                locations.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                locations.setSyncCount(record.GetSyncCount());
                locations.setSynced(true);
                locations.setUpdated(true);
            }
            else if("controllers" == record.GetKey()){
                controllers.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                controllers.setSyncCount(record.GetSyncCount());
                controllers.setSynced(true);
                controllers.setUpdated(true);
            }
            else if("shadeGroups" == record.GetKey()){
                shadeGroups.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                shadeGroups.setSyncCount(record.GetSyncCount());
                shadeGroups.setSynced(true);
                shadeGroups.setUpdated(true);
            }
        }
    }
    else{
        qDebug() << "Update request failed: " << updateResp.GetError().GetMessage().c_str();
    }

}

template<class T>
void CognitoSyncAPI::syncRecord(SyncableRecord<T> & syncable, const Model::Record & record, Model::UpdateRecordsRequest & update) const
{
    if(syncable.syncCount() == record.GetSyncCount() && !syncable.synced()){
        // local cache is fresher, do syncWithStale strategy
        Model::RecordPatch patch;
        patch.SetKey(record.GetKey());
        patch.SetValue(Aws::Utils::StringUtils::to_string(syncable.syncWithStale(record.GetValue().c_str()).toStdString()));
        patch.SetOp(Aws::CognitoSync::Model::Operation::replace);
        patch.SetSyncCount(record.GetSyncCount());
        update.AddRecordPatches(patch);
        syncable.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
        qDebug() << "SYNC WITH STALE [" << record.GetKey().c_str() << "] " << \
                    " SyncCount L~R: " << syncable.syncCount() << "~" << record.GetSyncCount();
    }
    else if(syncable.syncCount() != record.GetSyncCount()){
        // remote cache is fresher, do syncWithFresher strategy
        QString updString = syncable.syncWithFresher(record.GetValue().c_str());
        Model::RecordPatch patch;
        patch.SetKey(record.GetKey());
        patch.SetOp(Aws::CognitoSync::Model::Operation::replace);
        patch.SetSyncCount(record.GetSyncCount());
        patch.SetValue(Aws::Utils::StringUtils::to_string(updString.toStdString()));
        update.AddRecordPatches(patch);
        syncable.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
        qDebug() << "SYNC WITH FRESHER [" << record.GetKey().c_str() << "] " << \
                    " SyncCount L~R: " << syncable.syncCount() << "~" << record.GetSyncCount();
        syncable.setSyncCount(record.GetSyncCount());
    }
    else {
        syncable.setUpdated(false);
        syncable.setSynced(true);
        qDebug() << "SYNC MATCH [" << record.GetKey().c_str() << "] => " << record.GetValue().c_str() << \
                    " with syncCount: " << record.GetSyncCount();
    }
}

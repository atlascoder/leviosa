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
#include <QJsonObject>
#include <QJsonDocument>
#include <QMutexLocker>

#include <memory>

#include "core/LocalCache.h"
#include "aws/AwsApi.h"
#include "AlertBox.h"

using namespace Aws;
using namespace Aws::Auth;
using namespace std;

void SyncCache(LocalCache<Syncable>& cache, const QString& uuid, const QString& jsonValue, long long syncCount, CognitoSync::Model::UpdateRecordsRequest& updateRemoteRequest)
{
    // CognitoSync does return empty JSONs for deleted items
    // Such items should be deleted in local cache
    if (jsonValue.isEmpty() || jsonValue == "{}") {
        // delete from local cache
        cache.deleteItem(uuid, syncCount);
        return;
    }

    // Walk through local cache and collate items by SyncCount
    // Remote cache is requesting with LastSyncCount and response contains unly updated records
    // local SyncCount == -1 means that item is deleted in local cache and should be deleted in remote cache
    // remote-SyncCount > local-SyncCount means that record was updated remotely and this change has precedence over local change
    // SyncCount that equals in local and remote cache means that rremote record is not changed
    // if local isSynced is false - local data was changed and requires to change in remote
    Syncable* item;
    if (cache.findByKey(uuid, &item)) {
        if (item->syncCount() == -1) {
            // delete from remote cache
            CognitoSync::Model::RecordPatch patch;
            patch.SetKey(uuid.toStdString());
            patch.SetOp(CognitoSync::Model::Operation::remove);
            patch.SetSyncCount(syncCount);
            updateRemoteRequest.AddRecordPatches(patch);
        }
        else if (item->syncCount() != syncCount){
            // update item in local cache, overwrite local change
            cache.updateItem(uuid, jsonValue, syncCount);
        }
        else if (item->isChanged()) {
            // update remote cache with local value
            CognitoSync::Model::RecordPatch patch;
            patch.SetKey(uuid.toStdString());
            patch.SetOp(CognitoSync::Model::Operation::replace);
            patch.SetSyncCount(syncCount);
//            patch.SetValue(QJsonDocument(item->toJson()).toJson().toStdString());
            updateRemoteRequest.AddRecordPatches(patch);
        }
    }
    else {
        // insert new item into local cache
        cache.createItem(uuid, jsonValue, syncCount);
    }

}

void CompleteUpdateRemoteRequest(LocalCache<Syncable>& record, CognitoSync::Model::UpdateRecordsRequest& updateRemoteRequest)
{
    for (auto& key : record.leftItemsKeys()) {
        auto i = find_if(
                    record.items().begin(),
                    record.items().end(),
                    [key](const unique_ptr<Syncable>& _i)->bool{ return key == _i->uuid(); }
                );
        if (i != record.items().end() && !i->get()->isDeleted()) {
            // create
            CognitoSync::Model::RecordPatch patch;
            patch.SetSyncCount(0);
            patch.SetKey(i->get()->uuid().toStdString());
//            patch.SetValue(QJsonDocument(i->get()->toJson()).toJson(QJsonDocument::JsonFormat::Compact).toStdString());
            patch.SetOp(CognitoSync::Model::Operation::replace);
            updateRemoteRequest.AddRecordPatches(patch);
        }
    }
}

long long getLastSyncCount(const shared_ptr<vector<shared_ptr<Syncable>>>& items) {
    if (items->empty())
        return 0;
    else
        return max_element(
                    items->begin(),
                    items->end(),
                    [](const shared_ptr<Syncable>& a, const shared_ptr<Syncable>& b)->bool{ return a->syncCount() < b->syncCount(); }
                )->get()->syncCount();
}


void CognitoSyncAPI::syncData(const shared_ptr<vector<shared_ptr<Syncable>>>& cacheList)
{
    long long lastSyncCount = getLastSyncCount(cacheList);
    qDebug() << "Sync started with cache, with syncCount " << lastSyncCount;
    mIsSuccessful = false;
    mIsUpdated = false;
    std::shared_ptr<CognitoSync::CognitoSyncClient> client = AwsApi::instance().getClient<CognitoSync::CognitoSyncClient>();
    if (client.get() == nullptr) return;
    CognitoSync::Model::ListRecordsRequest req;
    req.SetDatasetName("items");
    req.SetIdentityId(AwsApi::instance().getIdentityId());
    req.SetIdentityPoolId(ClientConfig::instance().identityPool);
    req.SetLastSyncCount(lastSyncCount);

    auto resp = client->ListRecords(req);
    if (resp.IsSuccess()) {
        // walk through remote cache looking for new, updated, deleted and should be deleted
        for (auto& r : resp.GetResult().GetRecords()) {
            if (strncmp(r.GetValue().c_str(), "{}", r.GetValue().size()) == 0) {
                // deleted record
                auto item = find_if(cacheList->begin(), cacheList->end(), [r](const shared_ptr<Syncable>& _item)->bool{ return r.GetKey()==_item->uuid().toStdString(); });
                if (item != cacheList->end() && item->get()->syncCount() != r.GetSyncCount()) {
                    // set deleted
                    item->get()->setDeleted(true);
                    item->get()->setValue("{}");
                    item->get()->setSyncCount(r.GetSyncCount());
                    item->get()->setChanged(false);
                    mIsUpdated = true;
                }
                else {
                    // create deleted record
                    shared_ptr<Syncable> item(new Syncable(QString(r.GetKey().c_str())));
                    item->setDeleted(true);
                    item->setValue("{}");
                    item->setSyncCount(r.GetSyncCount());
                    cacheList->push_back(item);
                }
            }
            else {
                // contented record
                auto item = find_if(cacheList->begin(), cacheList->end(), [r](const shared_ptr<Syncable>& _item)->bool{ return r.GetKey()==_item->uuid().toStdString(); });
                if (item != cacheList->end() && item->get()->syncCount() != r.GetSyncCount()) {
                    // update existing item
                    item->get()->setValue(QString(r.GetValue().c_str()));
                    item->get()->setSyncCount(r.GetSyncCount());
                    item->get()->setChanged(false);
                }
                else {
                    // create new item
                    shared_ptr<Syncable> item(new Syncable(QString(r.GetKey().c_str())));
                    item->setValue(QString(r.GetValue().c_str()));
                    item->setSyncCount(r.GetSyncCount());
                    cacheList->push_back(item);
                }
            }
        }

        CognitoSync::Model::UpdateRecordsRequest update_request;
        update_request.SetDatasetName("items");
        update_request.SetIdentityId(AwsApi::instance().getIdentityId());
        update_request.SetIdentityPoolId(ClientConfig::instance().identityPool);
        update_request.SetSyncSessionToken(resp.GetResult().GetSyncSessionToken());

        // Complete remote cache updateing requests addnig new items from local cache
        for (auto& c : *cacheList) {
            if (c->isChanged()) {
                CognitoSync::Model::RecordPatch patch;
                patch.SetKey(c->uuid().toStdString());
                patch.SetSyncCount(c->syncCount());
                if (c->isDeleted()) {
                    patch.SetOp(CognitoSync::Model::Operation::remove);
                }
                else {
                    patch.SetOp(CognitoSync::Model::Operation::replace);
                    patch.SetValue(c->value().toStdString());
                }
                update_request.AddRecordPatches(patch);
            }
        }

        if (update_request.GetRecordPatches().empty()) {
            mIsSuccessful = true;
            mIsUpdated = lastSyncCount != getLastSyncCount(cacheList);
            return;
        }
        CognitoSync::Model::UpdateRecordsOutcome update_response = client->UpdateRecords(update_request);
        if (update_response.IsSuccess()) {
            // update syncounts and persist
            for (auto& r : update_response.GetResult().GetRecords()) {
                QString uuid(r.GetKey().c_str());
                auto i = find_if(cacheList->begin(), cacheList->end(), [&uuid](const shared_ptr<Syncable> _i)->bool{ return _i->uuid()==uuid; });
                if (i != cacheList->end()) {
                    i->get()->setChanged(false);
                    i->get()->setSyncCount(r.GetSyncCount());
                }
            }
            mIsUpdated = lastSyncCount != getLastSyncCount(cacheList);
            mIsSuccessful = true;
        }
        else {
            mLastMessage = update_response.GetError().GetMessage().c_str();
            AlertBox::instance().showMessage(
                        "Cloud synchronization error",
                        mLastMessage,
                        ""
                        );
            qDebug() << "Error while updating r-cache: " << mLastMessage;
        }
    }
    else {
        mLastMessage = resp.GetError().GetMessage().c_str();
        AlertBox::instance().showMessage(
                    "Cloud synchronization error",
                    mLastMessage,
                    ""
                    );
        qDebug() << "Error while getting remote cache: " << mLastMessage;
    }
}

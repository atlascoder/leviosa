#include "CognitoSyncAPI.h"
#include <aws/cognito-sync/CognitoSyncClient.h>
#include <aws/cognito-sync/model/ListRecordsRequest.h>
#include <aws/cognito-sync/model/ListRecordsResult.h>
#include <aws/cognito-sync/CognitoSyncRequest.h>
#include <aws/core/utils/Outcome.h>
#include <aws/cognito-sync/model/UpdateRecordsResult.h>
#include <aws/cognito-sync/model/RecordPatch.h>

#include <QDebug>

#include "ClientConfig.h"
#include "core/Syncable.h"
#include <memory>

using namespace Aws::CognitoSync;
using namespace std;

void CognitoSyncAPI::sync(const Aws::Auth::AWSCredentials & credentials, const QString & datasetName, vector<unique_ptr<Syncable>> * localDataset, std::vector<std::unique_ptr<Syncable>> * out, std::vector<int> * deleted){
    Model::ListRecordsRequest req;
    req.SetDatasetName(datasetName.toStdString());
    req.SetIdentityId(ClientConfig::instance().identityId);
    req.SetIdentityPoolId(ClientConfig::instance().identityPool);

    CognitoSyncClient client(credentials, ClientConfig::instance());

    // indexes of local records that is not require for upload
    set<int> syncedIds;
    map<int, int> toDelete;

    // receive remote data and mark updated by lastModifiedDate
    auto resp = client.ListRecords(req);
    if(resp.IsSuccess()){
        const Aws::Vector<Model::Record> & records = resp.GetResult().GetRecords();
        for(auto& record : records){
            int id = stoi(record.GetKey().c_str());
            vector<int>::iterator d = find_if(deleted->begin(), deleted->end(), [id](const int & v) -> bool { return v==id; });
            if(d != deleted->end()){
                toDelete[id] = record.GetSyncCount();
                continue;
            }
            vector<unique_ptr<Syncable>>::iterator m = find_if(localDataset->begin(), localDataset->end(), [id](const unique_ptr<Syncable> & m) -> bool { return m.get()->id()==id; });
            if( m != localDataset->end()){
                // record exists locally
                Syncable *model = m->get();
                // delta time: 0 - equal, <0 - remote is newer, >0 - local is newer
                int delta = model->lastModified() - (record.GetLastModifiedDate().Millis() / 1000);
                if(delta < 0){
                    // update in localDataset, due remote record is newer
                    model->setSyncContent(QString(record.GetValue().c_str()));
                    model->setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                    model->setUpdated(true);
                    // no needs to upload this record again
                    syncedIds.insert(id);
                }
                else if(delta > 0){
                    // set correct sync for upload
                    model->setSyncs(record.GetSyncCount());
                }
                else {
                    // record is the same, no need to send
                    syncedIds.insert(id);
                }
            }
            else{
                // record not exist locally. Creating and adding to <outs>
                // checkif record has valid value and create record then
                if(record.GetValue().size() > 0){
                    unique_ptr<Syncable> mnew(new Syncable());
                    mnew->setId(id);
                    mnew->setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                    mnew->setSyncContent(QString(record.GetValue().c_str()));
                    mnew->setUpdated(true);
                    out->push_back(move(mnew));
                }
                else{
                    toDelete[id] = record.GetSyncCount();
                }
                // no need to send again
                syncedIds.insert(id);
            }
        }
    }
    else{
        qDebug() << "Error receiving Locations: " << resp.GetError().GetMessage().c_str();
    }

    // just return if nothing to upload
    if(syncedIds.size() == (localDataset->size() + out->size()) && toDelete.size()==0) return;

    Model::UpdateRecordsRequest updateRequest;

    for(auto update = localDataset->begin(); update != localDataset->end(); ++update){
        if(syncedIds.find(update->get()->id()) != syncedIds.end()) continue;
        Model::RecordPatch patch;
        patch.SetOp(Aws::CognitoSync::Model::Operation::replace);
        patch.SetKey(update->get()->syncKey().toStdString());
        patch.SetValue(update->get()->syncContent().toStdString());
        patch.SetSyncCount(update->get()->syncs());
        updateRequest.AddRecordPatches(patch);
    }

    for(auto to_del : toDelete){
        Model::RecordPatch patch;
        patch.SetOp(Aws::CognitoSync::Model::Operation::remove);
        patch.SetKey(Aws::String(QString::number(to_del.first).toStdString()));
        patch.SetSyncCount(to_del.second);
        updateRequest.AddRecordPatches(patch);
    }

    if(updateRequest.GetRecordPatches().size() == 0) return;

    updateRequest.SetIdentityId(ClientConfig::instance().identityId);
    updateRequest.SetIdentityPoolId(ClientConfig::instance().identityPool);
    updateRequest.SetDatasetName(datasetName.toStdString());
    updateRequest.SetSyncSessionToken(resp.GetResult().GetSyncSessionToken());

    deleted->clear();

    auto updateResp = client.UpdateRecords(updateRequest);
    if(updateResp.IsSuccess()){
        for(auto& update : updateResp.GetResult().GetRecords()){
            int id = stoi(update.GetKey().c_str());
            if(toDelete.find(id) != toDelete.end()){
                deleted->push_back(id);
                continue;
            }
            vector<unique_ptr<Syncable>>::iterator m = find_if(localDataset->begin(), localDataset->end(), [id](const unique_ptr<Syncable> & x) -> bool { return x->id()==id; });
            // set syncs in local cache
            if(m != localDataset->end()){
                m->get()->setLastModified(update.GetLastModifiedDate().Millis() / 1000);
                m->get()->setSyncs(update.GetSyncCount());
                m->get()->setUpdated(true);
            }
            m = find_if(out->begin(), out->end(), [id](const unique_ptr<Syncable> & x) -> bool { return x->id()==id; });
            // set syncs in out cache
            if(m != out->end()){
                m->get()->setLastModified(update.GetLastModifiedDate().Millis() / 1000);
                m->get()->setSyncs(update.GetSyncCount());
                m->get()->setUpdated(true);
            }
        }
    }
    else{
        qDebug() << "Update request failed: " << updateResp.GetError().GetMessage().c_str();
    }
}

void CognitoSyncAPI::sync(const Aws::Auth::AWSCredentials & credentials, const QString & datasetName, Syncable & locations, Syncable & controllers, Syncable & shadeGroups)
{
    Model::ListRecordsRequest req;
    req.SetDatasetName(datasetName.toStdString());
    req.SetIdentityId(ClientConfig::instance().identityId);
    req.SetIdentityPoolId(ClientConfig::instance().identityPool);
    Model::UpdateRecordsRequest updateRequest;

    CognitoSyncClient client(credentials, ClientConfig::instance());

    // receive remote data and mark updated by lastModifiedDate
    auto resp = client.ListRecords(req);
    if(resp.IsSuccess()){
        const Aws::Vector<Model::Record> & records = resp.GetResult().GetRecords();
        for(auto& record : records){
            if("locations" == record.GetKey()){
                syncRecord(locations, record, updateRequest);
            }
            else if("controllers" == record.GetKey()){
                syncRecord(controllers, record, updateRequest);
            }
            else if("shadowGroups" == record.GetKey()){
                syncRecord(shadeGroups, record, updateRequest);
            }
        }
    }
    else{
        qDebug() << "Error receiving Locations: " << resp.GetError().GetMessage().c_str();
    }

    if(updateRequest.GetRecordPatches().size() == 0) return;

    updateRequest.SetIdentityId(ClientConfig::instance().identityId);
    updateRequest.SetIdentityPoolId(ClientConfig::instance().identityPool);
    updateRequest.SetDatasetName(datasetName.toStdString());
    updateRequest.SetSyncSessionToken(resp.GetResult().GetSyncSessionToken());

    auto updateResp = client.UpdateRecords(updateRequest);
    if(updateResp.IsSuccess()){
        const Aws::Vector<Model::Record> & records = resp.GetResult().GetRecords();
        for(auto& record : records){
            if("locations" == record.GetKey() && locations.lastModified() != record.GetLastModifiedDate().Millis()/1000){
                locations.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                locations.setSyncs(record.GetSyncCount());
                locations.setUpdated(true);
            }
            else if("controllers" == record.GetKey() && controllers.lastModified() != record.GetLastModifiedDate().Millis()/1000){
                controllers.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                controllers.setSyncs(record.GetSyncCount());
                controllers.setUpdated(true);
            }
            else if("shadowGroups" == record.GetKey() && shadeGroups.lastModified() != record.GetLastModifiedDate().Millis()/1000){
                shadeGroups.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
                shadeGroups.setLastModified(record.GetSyncCount());
                shadeGroups.setUpdated(true);
            }
        }
    }
    else{
        qDebug() << "Update request failed: " << updateResp.GetError().GetMessage().c_str();
    }

}

void CognitoSyncAPI::syncRecord(Syncable & syncable, const Model::Record & record, Model::UpdateRecordsRequest & update) const
{
    int delta = syncable.lastModified() - record.GetLastModifiedDate().Millis()/1000;
    if(delta < 0){
        // local locationse stale
        syncable.setSyncContent(record.GetValue().c_str());
        syncable.setSyncs(record.GetSyncCount());
        syncable.setLastModified(record.GetLastModifiedDate().Millis() / 1000);
        syncable.setUpdated(true);
    }
    else if(delta > 0){
        // local data is newer
        syncable.setSyncs(record.GetSyncCount());
        Model::RecordPatch patch;
        patch.SetKey(record.GetKey());
        patch.SetOp(Aws::CognitoSync::Model::Operation::replace);
        patch.SetSyncCount(record.GetSyncCount());
        patch.SetValue(syncable.syncContent().toStdString());
        update.AddRecordPatches(patch);
    }
    else {
        syncable.setUpdated(false);
    }
}

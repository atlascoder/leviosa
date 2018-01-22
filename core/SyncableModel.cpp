#include "SyncableModel.h"
#include <memory>
#include <QtConcurrentRun>
#include <CurrentUser.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>

using namespace std;

SyncableModel::SyncableModel(const QString& datasetName) :
    mDatasetName(datasetName),
    mSyncResult(),
    mInProgress(false),
    mLocalDataCache(new vector<unique_ptr<Syncable>>),
    mUpdates(new vector<unique_ptr<Syncable>>),
    mDeleted(new vector<int>),
    mLocalData(0)
{

}

SyncableModel::~SyncableModel()
{
    if(mSyncResult.isRunning())
        mSyncResult.waitForFinished();
}

void SyncableModel::sync()
{
    if(mInProgress)
        return;
    else
        setSyncing(true);

    if(mSyncResult.isCanceled() || mSyncResult.isFinished())
    {
        mSyncResult = QtConcurrent::run(this, &SyncableModel::syncRequest);
    }
}

void SyncableModel::syncRequest()
{
    // copying current data locally
    mLocalDataCache->clear();
    int n = mLocalData->size();
    for(int i = 0; i < n; i++){
        unique_ptr<Syncable> s(new Syncable());
        Syncable* item = mLocalData->at(i).get();
        s->setId(item->id());
        s->setLastModified(item->lastModified());
        s->setSyncContent(item->syncContent());
        s->setSyncs(item->syncs());
        s->setUpdated(item->lastModified() == 0 ? true : false);
        mLocalDataCache->push_back(move(s));
    }
    mUpdates->clear();

    CognitoSyncAPI syncer;
    CurrentUser & u = CurrentUser::instance();
    u.requestCredentials();
    if(u.isAuthenticated() && u.hasCredentials()){
        Aws::Auth::AWSCredentials creds;
        u.fillCredentials(creds);
        syncer.sync(creds, mDatasetName, mLocalDataCache.get(), mUpdates.get(), mDeleted.get());
        mergeUpdates();
    }
    setSyncing(false);
}

void SyncableModel::mergeUpdates()
{
    // clear cache
    mLocalDataCache->clear();
    mUpdates->clear();
}

bool SyncableModel::setSyncing(bool isSyncing)
{
    if(isSyncing == mInProgress) return false;
    mInProgress = isSyncing;
    return true;
}

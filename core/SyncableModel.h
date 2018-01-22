#ifndef SYNCABLEMODEL_H
#define SYNCABLEMODEL_H

#include <QFuture>
#include "Syncable.h"
#include "aws/CognitoSyncAPI.h"

class SyncableModel
{
    QString mDatasetName;
    QFuture<void> mSyncResult;
    bool mInProgress;
public:
    SyncableModel(const QString& datasetName);
    virtual ~SyncableModel();

    virtual void sync();
    bool isSyncing() const { return mInProgress; }
protected:
    std::unique_ptr<std::vector<std::unique_ptr<Syncable>>> mLocalDataCache;
    std::unique_ptr<std::vector<std::unique_ptr<Syncable>>> mUpdates;
    std::unique_ptr<std::vector<int>> mDeleted;
    std::vector<std::unique_ptr<Syncable>>* mLocalData;
    void syncRequest();
    virtual void mergeUpdates()=0;
    virtual bool setSyncing(bool isSyncing);
};

#endif // SYNCABLEMODEL_H

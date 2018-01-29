#ifndef SYNCABLERECORD_H
#define SYNCABLERECORD_H

#include <QString>
#include <QJsonObject>
#include <memory>
#include <vector>

using namespace std;

template<class T>
class SyncableRecord
{
    int mSyncCount;
    bool mUpdated;
    bool mSynced;
    int mLastModified;
    unique_ptr<vector<unique_ptr<T>>> mItems;
    vector<int> mDeletedIdxs;
public:
    SyncableRecord(const vector<unique_ptr<T>>& localData, int lastModified, int syncCount, bool isSynced) :
        mSyncCount(syncCount),
        mUpdated(false),
        mSynced(isSynced),
        mLastModified(lastModified),
        mItems(new vector<unique_ptr<T>>),
        mDeletedIdxs()
    {
        for(auto i = localData.begin(); i != localData.end(); i++){
            unique_ptr<T> t(new T(*i->get()));
            mItems->push_back(move(t));
        }
    }

    int syncCount() const { return mSyncCount; }
    void setSyncCount(int syncs) { mSyncCount = syncs; }

    int lastModified() const { return mLastModified; }
    void setLastModified(int lastModified) { mLastModified = lastModified; }

    // Syncable content is differs from initial
    // persist required
    bool updated() const { return mUpdated; }
    void setUpdated(bool isUpdated) { mUpdated = isUpdated; }

    // Syncable was synced
    // lastModified was changed
    bool synced() const { return mSynced; }
    void setSynced(bool isSynced) { mSynced = isSynced; }

    QString syncContent() const;
    void setSyncContent(const QString & syncContent);

    QString syncWithStale(const QString& remoteCache);
    QString syncWithFresher(const QString& remoteCache);

    vector<int>& deletedIdx() { return mDeletedIdxs; }

    vector<unique_ptr<T>>& items() const {return *mItems.get();}
private:
    QString recordKey(const T& item) const;
    bool findByKey(const QString& key, T** pFound) const;
    std::unique_ptr<T> fromJson(const QString& key, const QJsonObject& json) const;

};

#include "SyncableRecord.tpp"
#endif // SYNCABLERECORD_H

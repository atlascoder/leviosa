#ifndef SYNCABLE_H
#define SYNCABLE_H

#include <QString>

class Syncable
{
    int mId;
    int mSyncs;
    int mLastModified;
    bool mUpdated;
    QString mSyncContent;
public:
    Syncable() : mId(-1), mSyncs(0), mLastModified(0), mUpdated(false), mSyncContent("{}") {}

    int id() const { return mId; }
    void setId(int id) { mId = id; }

    int syncs() const { return mSyncs; }
    void setSyncs(int syncs) { mSyncs = syncs; }

    int lastModified() const { return mLastModified; }
    void setLastModified(int lastModified) { mLastModified = lastModified; }

    bool updated() const { return mUpdated; }
    void setUpdated(bool isUpdated) { mUpdated = isUpdated; }

    virtual QString syncContent() const { return mSyncContent; }
    virtual void setSyncContent(const QString & syncContent) { mSyncContent = syncContent; }

    QString syncKey() const { return QString::number(mId); }

};

#endif // SYNCABLE_H

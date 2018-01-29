#include "SyncableRecord.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

template<class T>
QString SyncableRecord<T>::syncContent() const
{
    QJsonObject json;
    auto i = mItems->begin();
    while(i != mItems->end()){
        json.insert(recordKey(*i->get()), i->get()->toJson());
        i++;
    }
    QString s = QJsonDocument(json).toJson(QJsonDocument::Compact);
    qDebug() << "Locations syncable generated: " << s << "// lastModified: " << mLastModified;
    return s;
}

template<class T>
QString SyncableRecord<T>::syncWithStale(const QString &remoteCache)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(remoteCache.toStdString()), &error);
    QJsonObject update;
    if(error.error == QJsonParseError::NoError){
        QJsonObject obj = doc.object();
        T* pItem = nullptr;
        // walk over remote cache to copy updated and remote new
        for(QString key : obj.keys()){
            if(findByKey(key, &pItem)){
                // copy local to update
                update.insert(key, pItem->toJson());
            }
            else{
                // passed away record that absent locally
            }
        }
        // walk over local cache to find deleted and new
        for(size_t idx = 0; idx < mItems->size(); idx++){
            if(!obj.contains(recordKey(*mItems->at(idx).get()))){
                if(mItems->at(idx)->isSynced()){
                    mDeletedIdxs.push_back(idx);
                }
                else{
                    update.insert(recordKey(*mItems->at(idx).get()), mItems->at(idx)->toJson());
                }
            }
        }
    }
    else{
        qDebug() << "JSON error: " << error.errorString() << " // JSON: " << remoteCache;
    }
    return QJsonDocument(update).toJson(QJsonDocument::Compact);
}

template<class T>
QString SyncableRecord<T>::syncWithFresher(const QString &remoteCache)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(remoteCache.toStdString()), &error);
    QJsonObject update;
    if(error.error == QJsonParseError::NoError){
        mUpdated = true;
        mSynced = true;
        QJsonObject obj = doc.object();
        T* pItem = nullptr;
        // walk over remote cache to copy updated and remote new
        for(QString key : obj.keys()){
            if(findByKey(key, &pItem)){
                // copy local to update
                pItem->withJson(obj[key].toObject());
                update.insert(key, obj[key]);
            }
            else{
                // copy to cache as updated
                unique_ptr<T> t(fromJson(key, obj[key].toObject()));
                update.insert(key, obj[key]);
                mItems->push_back(move(t));
            }
        }
        // walk over cache to find deleted and new
        for(size_t idx = 0; idx < mItems->size(); idx++){
            if(!obj.contains(recordKey(*mItems->at(idx).get()))){
                if(mItems->at(idx)->isSynced()){
                    mDeletedIdxs.push_back(idx);
                }
                else{
                    mSynced = false;
                    update.insert(recordKey(*mItems->at(idx).get()), mItems->at(idx)->toJson());
                }
            }
        }
    }
    else{
        qDebug() << "JSON error: " << error.errorString() << " // JSON: " << remoteCache;
    }
    return QJsonDocument(update).toJson(QJsonDocument::Compact);
}

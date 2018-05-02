#ifndef LOCALCACHE_H
#define LOCALCACHE_H

#include <QString>
#include <QJsonObject>
#include <memory>
#include <vector>

using namespace std;

template<class T>
class LocalCache
{
    QString mTableName;
    bool mUpdated;
    vector<unique_ptr<T>>* mItems;
    unique_ptr<vector<QString>> mLeftKeys;
public:
    LocalCache(const QString& tableName, vector<unique_ptr<T>>* localData) :
        mTableName(tableName),
        mUpdated(false),
        mItems(localData),
        mLeftKeys(new vector<QString>)
    {
        for(auto i = localData->begin(); i != localData->end(); i++){
            mLeftKeys->push_back(i->get()->uuid());
        }
    }

    // Syncable content is differs from initial
    // persist required
    bool updated() const { return mUpdated; }
    void setUpdated(bool isUpdated) { mUpdated = isUpdated; }

    vector<unique_ptr<T>>& items() const { return *mItems; }

    void createItem(const QString& uuid, const QString& jsonValue, long long syncCount);

    void updateItem(const QString& uuid, const QString& jsonValue, long long syncCount);

    void deleteItem(const QString& uuid, long long syncCount);

    vector<QString>& leftItemsKeys() const { return *mLeftKeys.get(); }

    bool findByKey(const QString& key, T** pFound) const;

    QString tableName() const { return mTableName; }

private:
    QString recordKey(const T& item) const;
    std::unique_ptr<T> fromJson(const QString& key, const QJsonObject& json) const;

};

#include "LocalCache.tpp"
#endif // LOCALCACHE_H

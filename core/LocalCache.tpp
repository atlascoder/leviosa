#include "LocalCache.h"
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

template<class T>
QString LocalCache<T>::recordKey(const T& item) const
{
    return item.uuid();
}

template<class T>
std::unique_ptr<T> LocalCache<T>::fromJson(const QString& key, const QJsonObject &json) const
{
    std::unique_ptr<T> item(new T(key));
    item->withJson(json);
    return item;
}

template<class T>
bool LocalCache<T>::findByKey(const QString &key, T** pFound) const
{
    auto found = find_if(mItems->begin(), mItems->end(), [key](const unique_ptr<T>& i)->bool{ return key==i->uuid(); });
    if(found != mItems->end()){
        *pFound = found->get();
        auto k = find_if(
                    mLeftKeys->begin(),
                    mLeftKeys->end(),
                    [&found](const QString& _k)->bool{ return _k == found->get()->uuid(); }
        );
        if (k != mLeftKeys->end()) mLeftKeys->erase(k);
        return true;
    }
    else return false;
}

template<class T>
void LocalCache<T>::createItem(const QString &uuid, const QString &jsonValue, long long syncCount)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(jsonValue.toStdString()), &error);
    if (error.error == QJsonParseError::NoError) {
        std::unique_ptr<T> item(new T(uuid));
//        item->withJson(doc.object());
        item->setSyncCount(syncCount);
        mItems->push_back(std::move(item));
    }
    else {
        qDebug() << "CognitoSync: create item parsing " << uuid << " => " << jsonValue << " with error: " << error.errorString();
    }
}

template<class T>
void LocalCache<T>::deleteItem(const QString &uuid, long long syncCount)
{
    auto item = std::find_if(mItems->begin(), mItems->end(), [uuid](const std::unique_ptr<T>& _i)->bool{ return _i->uuid()==uuid; });
    if (item != mItems->end()) {
        item->get()->setSyncCount(syncCount);
//        item->get()->setDeleted();
    }
    else {
        std::unique_ptr<T> new_item(new T(uuid));
//        new_item->setDeleted();
        new_item->setSyncCount(syncCount);
        mItems->push_back(std::move(new_item));
    }
}

template<class T>
void LocalCache<T>::updateItem(const QString &uuid, const QString &jsonValue, long long syncCount)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(jsonValue.toStdString()), &error);
    if (error.error == QJsonParseError::NoError) {
        auto item = std::find_if(mItems->begin(), mItems->end(), [uuid](const std::unique_ptr<T>& _i)->bool{ return _i->uuid()==uuid; });
        if (item != mItems->end()) {
//            item->get()->withJson(doc.object());
            item->get()->setSyncCount(syncCount);
        }
        else {
            std::unique_ptr<T> new_item(new T(uuid));
//            new_item->withJson(doc.object());
            new_item->setSyncCount(syncCount);
            mItems->push_back(std::move(new_item));
        }
    }
    else {
        qDebug() << "CognitoSync: update item parsing " << uuid << " => " << jsonValue << " with error: " << error.errorString();
    }
}

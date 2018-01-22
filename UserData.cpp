#include "UserData.h"
#include "aws/CredentialsRequest.h"
#include "aws/CognitoSyncAPI.h"
#include "CurrentUser.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

using namespace std;

UserData& UserData::instance()
{
    static UserData sInstance;
    return sInstance;
}

void UserData::run()
{
    if(mSyncState == Syncing || mSyncState == Editing)
        return;
    else
        mSyncState = Syncing;

    CognitoSyncAPI syncer;
    CurrentUser & u = CurrentUser::instance();
    u.requestCredentials();
    if(u.isAuthenticated() && u.hasCredentials()){
        Aws::Auth::AWSCredentials creds;
        u.fillCredentials(creds);
        Syncable locations = locationsSyncable();
        Syncable controllers = controllersSyncable();
        Syncable shadeGroups = shadeGroupsSyncable();
        syncer.sync(creds, "config", locations, controllers, shadeGroups);
        mergeUpdates(locations, controllers, shadeGroups);
    }
    mSyncState = Clean;
}

UserData::~UserData()
{
    this->quit();
    this->wait();
}

//QString UserData::jsonValue() const
//{
//    QJsonObject locations;
//    for(UserLocation* location : *mLocations.get()){
//        locations.insert(location->bssid(), QJsonValue(location->syncContent()));
//    }
//    QJsonObject controllers;
//    for(LocationController* controller : *mControllers.get()){
//        controllers.insert(controller->mac(), QJsonValue(controller->syncContent()));
//    }
//    QJsonObject shadeGroups;
//    for(ShadeGroup* group : *mShadeGroups.get()){
//        shadeGroups.insert(group->channel(), group->syncContent());
//    }
//    QJsonObject root;
//    root.insert("locations", locations);
//    root.insert("controllers", controllers);
//    root.insert("shadeGroups", shadeGroups);
//    QJsonDocument doc(root);
//    return doc.toJson(QJsonDocument::Compact);
//}

Syncable UserData::locationsSyncable() const
{
    QJsonObject json;
    vector<unique_ptr<UserLocation>>::iterator i = mLocations->begin();
    while(i != mLocations->end()){
        json.insert(i->get()->bssid(), i->get()->syncContent());
        i++;
    }
    Syncable s;
    s.setSyncContent(QJsonDocument(json).toJson(QJsonDocument::Compact));
    s.setLastModified(mDb.locationsDao.lastModified());
    return s;
}

Syncable UserData::controllersSyncable() const
{
    QJsonObject json;
    vector<unique_ptr<LocationController>>::iterator i = mControllers->begin();
    while(i != mControllers->end()){
        json.insert(i->get()->mac(), i->get()->syncContent());
        i++;
    }
    Syncable s;
    s.setSyncContent(QJsonDocument(json).toJson(QJsonDocument::Compact));
    s.setLastModified(mDb.controllersDao.lastModified());
    return s;
}

Syncable UserData::shadeGroupsSyncable() const
{
    QJsonObject json;
    vector<unique_ptr<ShadeGroup>>::iterator i = mShadeGroups->begin();
    while(i != mShadeGroups->end()){
        json.insert(QString::number(i->get()->channel()), i->get()->syncContent());
        i++;
    }
    Syncable s;
    s.setSyncContent(QJsonDocument(json).toJson(QJsonDocument::Compact));
    s.setLastModified(mDb.shadeGroupsDao.lastModified());
    return s;
}

void UserData::mergeUpdates(Syncable &locations, Syncable &controllers, Syncable &shadeGroups)
{
    QJsonDocument doc;
    QJsonParseError error;
    if(locations.updated()){
        doc = QJsonDocument::fromJson(QByteArray::fromStdString(locations.syncContent().toStdString()), &error);
        if(error.error == QJsonParseError::NoError && doc.isObject()){
            QJsonObject obj = doc.object();
            for(QJsonObject::iterator i = obj.begin(); i != obj.end(); i++){
                UserLocation l;
                l.setSyncContent(i.value().toString());
                mDb.locationsDao.persistItem(l);
            }
        }
        else{
            qDebug() << "JSON Parsing error: " << error.errorString() << " // JSON: " << locations.syncContent();
        }
    }

    if(controllers.updated()){
        doc = QJsonDocument::fromJson(QByteArray::fromStdString(controllers.syncContent().toStdString()), &error);
        if(error.error == QJsonParseError::NoError && doc.isObject()){
            QJsonObject obj = doc.object();
            for(QJsonObject::iterator i = obj.begin(); i != obj.end(); i++){
                LocationController c;
                c.setSyncContent(i.value().toString());
                mDb.controllersDao.persistItem(c);
            }
        }
        else{
            qDebug() << "JSON Parsing error: " << error.errorString() << " // JSON: " << locations.syncContent();
        }
    }

    if(shadeGroups.updated()){
        doc = QJsonDocument::fromJson(QByteArray::fromStdString(shadeGroups.syncContent().toStdString()), &error);
        if(error.error == QJsonParseError::NoError && doc.isObject()){
            QJsonObject obj = doc.object();
            for(QJsonObject::iterator i = obj.begin(); i != obj.end(); i++){
                ShadeGroup c;
                c.setSyncContent(i.value().toString());
                mDb.shadeGroupsDao.persistItem(c);
            }
        }
        else{
            qDebug() << "JSON Parsing error: " << error.errorString() << " // JSON: " << locations.syncContent();
        }
    }
    if(locations.updated() || controllers.updated() || shadeGroups.updated()) emit dataChanged();
}

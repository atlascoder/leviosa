#include "UserData.h"
#include "aws/CredentialsRequest.h"
#include "aws/CognitoSyncAPI.h"
#include "CurrentUser.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

#include <QDebug>

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
        setState(Syncing);

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
    setState(Clean);
}

UserData::~UserData()
{
    this->quit();
    this->wait();
}

Syncable UserData::locationsSyncable() const
{
    QJsonObject json;
    vector<unique_ptr<UserLocation>>::iterator i = mLocations->begin();
    while(i != mLocations->end()){
        json.insert(i->get()->uuid().toString(), i->get()->toJson());
        i++;
    }
    Syncable s;
    s.setSyncContent(QJsonDocument(json).toJson(QJsonDocument::Compact));
    s.setLastModified(mUser.locationsModified());
    qDebug() << "Locations syncable generated: " << s.syncContent() << "// lastModified: " << s.lastModified();
    return s;
}

Syncable UserData::controllersSyncable() const
{
    QJsonObject json;
    vector<unique_ptr<LocationController>>::iterator i = mControllers->begin();
    while(i != mControllers->end()){
        json.insert(i->get()->mac(), i->get()->toJson());
        i++;
    }
    Syncable s;
    s.setSyncContent(QJsonDocument(json).toJson(QJsonDocument::Compact));
    s.setLastModified(mUser.controllersModified());
    qDebug() << "Controllers syncable generated: " << s.syncContent() << "// lastModified: " << s.lastModified();
    return s;
}

Syncable UserData::shadeGroupsSyncable() const
{
    QJsonObject json;
    vector<unique_ptr<ShadeGroup>>::iterator i = mShadeGroups->begin();
    while(i != mShadeGroups->end()){
        json.insert(QString::number(i->get()->channel()), i->get()->toJson());
        i++;
    }
    Syncable s;
    s.setSyncContent(QJsonDocument(json).toJson(QJsonDocument::Compact));
    s.setLastModified(mUser.shadeGroupsModified());
    qDebug() << "ShadeGroups syncable generated: " << s.syncContent() << "// lastModified: " << s.lastModified();
    return s;
}

void UserData::mergeUpdates(Syncable &locations, Syncable &controllers, Syncable &shadeGroups)
{
    QJsonDocument doc;
    QJsonParseError error;
    if(locations.updated()){
        doc = QJsonDocument::fromJson(QByteArray::fromStdString(locations.syncContent().toStdString()), &error);
        qDebug() << "Merging locations update: " << locations.syncContent() << "// lastModified: " << locations.lastModified();
        if(error.error == QJsonParseError::NoError && doc.isObject()){
            mDb.locationsDao.clear();
            QJsonObject obj = doc.object();
            for(QJsonObject::iterator i = obj.begin(); i != obj.end(); i++){
                UserLocation l(i.key());
                l.withJson(i.value().toObject());
                mDb.locationsDao.persistItem(l);
            }
            mUser.setLocationsModified(locations.lastModified());
        }
        else{
            qDebug() << "JSON Parsing error: " << error.errorString() << " // JSON: " << locations.syncContent();
        }
    }
    else{
        qDebug() << "Merging locations not required";
    }

    if(controllers.updated()){
        doc = QJsonDocument::fromJson(QByteArray::fromStdString(controllers.syncContent().toStdString()), &error);
        qDebug() << "Merging controllers update: " << locations.syncContent() << "// lastModified: " << locations.lastModified();
        if(error.error == QJsonParseError::NoError && doc.isObject()){
            QJsonObject obj = doc.object();
            mDb.controllersDao.clear();
            for(QJsonObject::iterator i = obj.begin(); i != obj.end(); i++){
                LocationController c(i.key());
                c.withJson(i.value().toObject());
                mDb.controllersDao.persistItem(c);
            }
            mUser.setControllersModified(controllers.lastModified());
        }
        else{
            qDebug() << "JSON Parsing error: " << error.errorString() << " // JSON: " << locations.syncContent();
        }
    }
    else{
        qDebug() << "Merging controllers not required";
    }

    if(shadeGroups.updated()){
        doc = QJsonDocument::fromJson(QByteArray::fromStdString(shadeGroups.syncContent().toStdString()), &error);
        qDebug() << "Merging shadeGroups update: " << locations.syncContent() << "// lastModified: " << locations.lastModified();
        if(error.error == QJsonParseError::NoError && doc.isObject()){
            QJsonObject obj = doc.object();
            mDb.shadeGroupsDao.clear();
            for(QJsonObject::iterator i = obj.begin(); i != obj.end(); i++){
                ShadeGroup c(i.key().toInt());
                c.withJson(i.value().toObject());
                mDb.shadeGroupsDao.persistItem(c);
            }
            mUser.setShadeGroupsModified(shadeGroups.lastModified());
        }
        else{
            qDebug() << "JSON Parsing error: " << error.errorString() << " // JSON: " << locations.syncContent();
        }
    }
    else{
        qDebug() << "Merging shadeGroups not required";
    }

    if(locations.updated() || controllers.updated() || shadeGroups.updated()){
        mUser.persistUserDataModified();
        emit dataUpdated();
    }
}

void UserData::sync()
{
    if(!isRunning())
        this->start();
}

void UserData::dataChanged()
{
    emit dataUpdated();
    sync();
}

void UserData::setState(SyncState state)
{
    if(state == mSyncState) return;
    mSyncState = state;
    emit syncStateChanged();
}

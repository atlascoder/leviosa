#include "locationdao.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QSqlError>
#include <QTextStream>

using namespace std;

LocationDao::LocationDao(QSqlDatabase &database):
    mDatabase(database)
{

}

void LocationDao::init() const
{
    if(!mDatabase.tables().contains("locations")){
        QSqlQuery query(mDatabase);
        query.exec("CREATE TABLE locations (" \
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
                    "name TEXT, " \
                    "bssid TEXT, " \
                    "ssid TEXT, " \
                    "utcOffset INTEGER, " \
                    "position INTEGER, " \
                    "syncs INTEGER DEFAULT 0, " \
                    "lastModified INTEGER DEFAULT 0, " \
                    "deleted INTEGER DEFAULT 0" \
                  ")");
        if(query.lastError().isValid())
            QTextStream(stdout) << "CREATE locations: " << query.lastError().text();
        else
            QTextStream(stdout) << "CREATE locations - OK";

    }
    else
        QTextStream(stdout) << "TABLE locations found.";
}

void LocationDao::addLocation(UserLocation &location) const
{
    QSqlQuery query(mDatabase);
    if(location.id() > 0 ){
        query.prepare("INSERT INTO locations(id, name, bssid, ssid, utcOffset, position, syncs, lastModified) VALUES (:id, :name, :bssid, :ssid, :utcOffset, :position, :syncs, :lastModified)");
        query.bindValue(":id", location.id());
    }
    else{
        query.prepare("INSERT INTO locations(name, bssid, ssid, utcOffset, position, syncs, lastModified) VALUES (:name, :bssid, :ssid, :utcOffset, :position, :syncs, :lastModified)");
    }

    query.bindValue(":name", location.name());
    query.bindValue(":bssid", location.bssid());
    query.bindValue(":ssid", location.ssid());
    query.bindValue(":utcOffset", location.utcOffset());
    query.bindValue(":position", location.position());
    query.bindValue(":syncs", location.syncs());
    query.bindValue(":lastModified", location.lastModified());
    query.exec();

    if(query.lastError().isValid())
        QTextStream(stdout) << "INS: " << query.lastError().text();
    else{
        QTextStream(stdout) << "location created: " << query.lastInsertId().toInt();
        location.setId(query.lastInsertId().toInt());
    }
}

void LocationDao::updateLocation(const UserLocation &location) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE locations SET name=:name, bssid=:bssid, ssid=:ssid, utcOffset=:utcOffset, position=:position, syncs=:syncs WHERE id=:id");
    query.bindValue(":name", location.name());
    query.bindValue(":bssid", location.bssid());
    query.bindValue(":ssid", location.ssid());
    query.bindValue(":utcOffset", location.utcOffset());
    query.bindValue(":position", location.position());
    query.bindValue(":id", location.id());
    query.bindValue(":syncs", location.syncs());
    query.bindValue(":lastModified", location.lastModified());

    query.exec();
    if(query.lastError().isValid())
        QTextStream(stdout) << "id: " << location.id() << " [" << query.lastError().nativeErrorCode() << "] "<< query.lastError().text();
    else{
        QTextStream(stdout) << "location (" << location.id() << ") updated: " << query.numRowsAffected();
    }
}

void LocationDao::removeLocation(UserLocation &location) const
{
    QSqlQuery query(mDatabase);
    query.prepare("DELETE FROM locations WHERE id=:id");
    query.bindValue(":id", location.id());
    location.setId(-1);
}

void LocationDao::markDeleted(int id) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE locations SET deleted=1 WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
}

unique_ptr<vector<int>> LocationDao::deletedIds() const
{
    QSqlQuery query(mDatabase);
    query.exec("SELECT id FROM locations WHERE deleted IS 1");
    unique_ptr<vector<int>> list(new vector<int>);
    while(query.next()){
        list->push_back(query.value("id").toInt());
    }
    return list;
}

unique_ptr<vector<unique_ptr<UserLocation>>> LocationDao::locations() const
{
    QSqlQuery query(mDatabase);
    query.exec("SELECT * FROM locations WHERE deleted IS 0 ORDER BY position ASC");
    if(query.lastError().isValid())
        QTextStream(stdout) << "SELECT locations error: " << query.lastError().text();
    unique_ptr<vector<unique_ptr<UserLocation>>> list(new vector<unique_ptr<UserLocation>>());
    while(query.next()){
        unique_ptr<UserLocation> loc(new UserLocation(query.value("name").toString()));
        loc->setId(query.value("id").toInt());
        loc->setBssid(query.value("bssid").toString());
        loc->setSsid(query.value("ssid").toString());
        loc->setUtcOffset(query.value("utcOffset").toInt());
        loc->setPosition(query.value("position").toInt());
        loc->setSyncs(query.value("syncs").toInt());
        loc->setLastModified(query.value("lastModified").toInt());
        list->push_back(move(loc));
    }
    return list;
}

void LocationDao::clear() const
{
    QSqlQuery query(mDatabase);
    query.exec("DELETE FROM locations");
}

void LocationDao::destroyDeleted(int id) const
{
    QSqlQuery query(mDatabase);
    query.prepare("DELETE FROM locations WHERE deleted IS 1 AND id=:id");
    query.bindValue(":id", id);
    query.exec();
}

void LocationDao::persistLocation(UserLocation &location) const
{
    QSqlQuery findLoc(mDatabase);
    findLoc.prepare("SELECT * FROM locations WHERE id=:id");
    findLoc.bindValue(":id", location.id());
    findLoc.exec();
    if(findLoc.first()){
        updateLocation(location);
    }
    else{
        addLocation(location);
    }
}

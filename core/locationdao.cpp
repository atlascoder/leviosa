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
                    "position INTEGER" \
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
    query.prepare("INSERT INTO locations(id, name, bssid, ssid, utcOffset, position) VALUES(NULL, :name, :bssid, :ssid, :utcOffset, :position)");
    query.bindValue(":name", location.name());
    query.bindValue(":bssid", location.bssid());
    query.bindValue(":ssid", location.ssid());
    query.bindValue(":utcOffset", location.utcOffset());
    query.bindValue(":position", location.position());
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
    query.prepare("UPDATE locations SET name=:name, bssid=:bssid, ssid=:ssid, utcOffset=:utcOffset, position=:position WHERE id=:id");
    query.bindValue(":name", location.name());
    query.bindValue(":bssid", location.bssid());
    query.bindValue(":ssid", location.ssid());
    query.bindValue(":utcOffset", location.utcOffset());
    query.bindValue(":position", location.position());
    query.bindValue(":id", location.id());
    query.exec();
    if(query.lastError().isValid())
        QTextStream(stdout) << "id: " << location.id() << " [" << query.lastError().nativeErrorCode() << "] "<< query.lastError().text();
    else{
        QTextStream(stdout) << "location (" << location.id() << ") updated: " << query.numRowsAffected();
    }
}

void LocationDao::removeLocation(UserLocation &location) const
{
    QSqlQuery query;
    query.prepare("DELETE FROM locations WHERE id=:id");
    query.bindValue(":id", location.id());
    location.setId(-1);
}

unique_ptr<vector<unique_ptr<UserLocation>>> LocationDao::locations() const
{
    QSqlQuery query;
    query.exec("SELECT * FROM locations ORDER BY position ASC");
    if(query.lastError().isValid())
        QTextStream(stdout) << "SELECT locations error: " << query.lastError().text();
    else
        QTextStream(stdout) << "locations found: " << query.size();
    unique_ptr<vector<unique_ptr<UserLocation>>> list(new vector<unique_ptr<UserLocation>>());
    while(query.next()){
        unique_ptr<UserLocation> loc(new UserLocation(query.value("name").toString()));
        loc->setId(query.value("id").toInt());
        loc->setBssid(query.value("bssid").toString());
        loc->setSsid(query.value("ssid").toString());
        loc->setUtcOffset(query.value("utcOffset").toInt());
        loc->setPosition(query.value("position").toInt());
        list->push_back(move(loc));
    }
    return list;
}

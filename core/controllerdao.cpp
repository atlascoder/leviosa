#include "controllerdao.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QSqlError>
#include <QTextStream>
#include <QSqlResult>

using namespace std;

ControllerDao::ControllerDao(QSqlDatabase& database):
    mDatabase(database)
{

}

void ControllerDao::init() const
{
    if(!mDatabase.tables().contains("controllers")){
        QSqlQuery query(mDatabase);
        query.exec(QString("CREATE TABLE location_controllers ( " \
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
                            "locationId INTEGER, " \
                            "name TEXT, " \
                            "mac TEXT, " \
                            "position INTEGER" \
                           ")"));
    }
}

void ControllerDao::addController(LocationController &controller) const
{
    QSqlQuery query;
    query.prepare("INSERT INTO location_controllers (locationId, name, mac, position) VALUES (:locationId, :name, :mac, :postition)");
    query.bindValue(":locationId", controller.locationId());
    query.bindValue(":name", controller.name());
    query.bindValue(":mac", controller.mac().toUpper());
    query.bindValue(":position", controller.position());
    query.exec();
    controller.setId(query.lastInsertId().toInt());
}

void ControllerDao::addControllerInLocation(int locationId, LocationController& controller) const
{
    controller.setLocationId(locationId);
    addController(controller);
}

void ControllerDao::removeController(int id) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM location_controllers WHERE id IS :id"));
    query.bindValue(":id", id);
    query.exec();
}

void ControllerDao::updateController(const LocationController &controller) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE location_controllers SET locationId = :locationId, name = :name, mac = :mac, position = :position WHERE id IS :id");
    query.bindValue(":locationId", controller.locationId());
    query.bindValue(":name", controller.name());
    query.bindValue(":mac", controller.mac().toUpper());
    query.bindValue(":position", controller.position());
    query.bindValue(":id", controller.id());
    query.exec();
    if(query.lastError().isValid())
        QTextStream(stdout) << "CTRLR update error: " << query.lastError().text() << endl;
    else
        QTextStream(stdout) << "CTRLR updated: " << controller.id();
}


void ControllerDao::removeControllersInLocation(int locationId) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM location_controllers WHERE locationId IS :locationId"));
    query.bindValue(":locationId", locationId);
    query.exec();
}

unique_ptr<vector<unique_ptr<LocationController>>> ControllerDao::controllersForLocation(int locationId) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("SELECT * FROM location_controllers WHERE locationId=:locationId ORDER BY position ASC"));
    query.bindValue(":locationId", locationId);
    query.exec();
    unique_ptr<vector<unique_ptr<LocationController>>> list(new vector<unique_ptr<LocationController>>());

    if(query.lastError().isValid()){
        QTextStream(stdout) << "CTRLR SELECT err: " << query.lastError().text() << endl;
        return list;
    }

    while (query.next()) {
        unique_ptr<LocationController> ctrlr(new LocationController(locationId));
        ctrlr->setId(query.value("id").toInt());
        ctrlr->setName(query.value("name").toString());
        ctrlr->setMac(query.value("mac").toString());
        ctrlr->setPosition(query.value("position").toInt());
        list->push_back(move(ctrlr));
    }
    return list;
}

bool ControllerDao::validateName(const QString &name, int locationId) const
{
    QSqlQuery query(mDatabase);
    query.prepare("SELECT id FROM location_controllers WHERE locationId=:locationId AND name=:name");
    query.bindValue(":locationId", locationId);
    query.bindValue(":name", name);
    query.exec();
    if(query.lastError().isValid()){
        QTextStream(stdout) << "CTRLR name vaildation `"<< name << "` err: " << query.lastError().text() << endl;
        return false;
    }
    else if(query.next()){
        QTextStream(stdout) << "CTRLR name vaildation `" << name << "`: " << "FAILED" << endl;
        return false;
    }
    else {
        QTextStream(stdout) << "CTRLR name vaildation `" << name << "`: " << "OK" << endl;
        return true;
    }
}

int ControllerDao::lastPosition(int locationId) const
{
    QSqlQuery query(mDatabase);
    query.prepare("SELECT MAX(position) AS last_pos FROM location_controllers WHERE locationId=:locationId");
    query.bindValue(":locationId", locationId);
    query.exec();
    if(query.lastError().isValid()){
        QTextStream(stdout) << "CTRLR last position err: " << query.lastError().text() << endl;
        return 0;
    }
    else if(query.next()){
        return query.value("last_pos").toInt();
    }
    else{
        return 0;
    }
}

bool ControllerDao::validateMac(const QString &mac) const
{
    QSqlQuery query(mDatabase);
    query.prepare("SELECT * FROM location_controllers WHERE mac=:mac");
    query.bindValue(":mac", mac.toUpper());
    query.exec();
    if(query.lastError().isValid()){
        QTextStream(stdout) << "CTRLR validate mac '" << mac << "` failed: " << query.lastError().text() << endl;
        return false;
    }
    else if(query.next()){
        QTextStream(stdout) << "CTRLR validate mac '" << mac << "` failed: DUPLICATION";
        return false;
    }
    else{
        QTextStream(stdout) << "CTRLR validate mac '" << mac << "` success." << endl;
        return true;
    }
}

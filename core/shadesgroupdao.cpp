#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QTextStream>
#include <QSqlError>

#include "shadesgroupdao.h"
#include "shadegroup.h"

using namespace std;

ShadesGroupDao::ShadesGroupDao(QSqlDatabase& database):
    mDatabase(database)
{

}

void ShadesGroupDao::init() const
{
    if(!mDatabase.tables().contains("controllers_shades_groups")){
        QSqlQuery query(mDatabase);
        query.exec(QString("CREATE TABLE controllers_shades_groups (" \
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
                                "controllerId INTEGER NOT NULL, " \
                                "name TEXT NOT NULL, " \
                                "channel INTEGER NOT NULL, " \
                                "position INTEGER, " \
                                "openAt INTEGER, " \
                                "closeAt INTEGER, " \
                                "days INTEGER, " \
                                "syncs INTEGER DEFAULT 0, " \
                                "lastModified INTEGER DEFAULT 0" \
                           ")"));
   }
}

void ShadesGroupDao::addShadesGroup(ShadeGroup& shadesGroup) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("INSERT INTO controllers_shades_groups (controllerId, name, position, openAt, closeAt, days, channel, syncs, lastModified) VALUES (:controllerId, :name, :position, :openAt, :closeAt, :days, :channel, :syncs, :lastModified)"));
    query.bindValue(":controllerId", shadesGroup.controllerMac());
    query.bindValue(":name", shadesGroup.name());
    query.bindValue(":position", shadesGroup.position());
    query.bindValue(":openAt", shadesGroup.openAt());
    query.bindValue(":closeAt", shadesGroup.closeAt());
    query.bindValue(":days", shadesGroup.days());
    query.bindValue(":channel", shadesGroup.channel());
    query.bindValue(":syncs", shadesGroup.syncs());
    query.bindValue(":lastModified", shadesGroup.lastModified());

    query.exec();
    if(query.lastError().isValid()){
        QTextStream(stdout) << "GRP add error: " << query.lastError().text() << endl;
    }
    else {
        QTextStream(stdout) << "GRP added for ctrlr `" << shadesGroup.controllerMac() << "` id: " << query.lastInsertId().toInt() << endl;
    }
    shadesGroup.setId(query.lastInsertId().toInt());
}

void ShadesGroupDao::addShadesGroupInController(int controllerId, ShadeGroup& shadesGroup) const
{
    shadesGroup.setControllerMac(controllerId);
    addShadesGroup(shadesGroup);
}

void ShadesGroupDao::removeShadesGroup(int id) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM controllers_shades_groups WHERE id IS :id"));
    query.bindValue(":id", id);
    query.exec();
}

void ShadesGroupDao::updateShadesGroup(const ShadeGroup &shadesGroup) const
{
    QSqlQuery query;
    query.prepare(QString("UPDATE controllers_shades_groups SET controllerId = :controllerId, name = :name, position = :position, openAt = :openAt, closeAt = :closeAt, days = :days, channel = :channel, syncs=:syncs, lastModified=:lastModified WHERE id IS :id"));
    query.bindValue(":id", shadesGroup.id());
    query.bindValue(":controllerId", shadesGroup.controllerMac());
    query.bindValue(":name", shadesGroup.name());
    query.bindValue(":position", shadesGroup.position());
    query.bindValue(":openAt", shadesGroup.openAt());
    query.bindValue(":closeAt", shadesGroup.closeAt());
    query.bindValue(":days", shadesGroup.days());
    query.bindValue(":channel", shadesGroup.channel());
    query.bindValue(":syncs", shadesGroup.syncs());
    query.bindValue(":lastModified", shadesGroup.lastModified());
    query.exec();
}

void ShadesGroupDao::removeShadesGroupsFromController(int controllerId) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM controllers_shades_groups WHERE controllerId IS :controllerId"));
    query.bindValue(":controllerId", controllerId);
    query.exec();
}

unique_ptr<vector<unique_ptr<ShadeGroup>>> ShadesGroupDao::shadesGroupsForController(int controllerId) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("SELECT * FROM controllers_shades_groups WHERE controllerId=:controllerId ORDER BY position ASC"));
    query.bindValue(":controllerId", controllerId);
    query.exec();
    if(query.lastError().isValid()){
        QTextStream(stdout) << "GRP sel for ctrlr `"<< controllerId <<"` err: " << query.lastError().text() << endl;
    }
    unique_ptr<vector<unique_ptr<ShadeGroup>>> list(new vector<unique_ptr<ShadeGroup>>());
    while(query.next()){
        unique_ptr<ShadeGroup> group(new ShadeGroup(controllerId));
        group->setId(query.value("id").toInt());
        group->setName(query.value("name").toString());
        group->setPosition(query.value("position").toInt());
        group->setOpenAt(query.value("openAt").toInt());
        group->setCloseAt(query.value("closeAt").toInt());
        group->setDays(query.value("days").toInt());
        group->setChannel((char)query.value("channel").toInt());
        group->setSyncs(query.value("syncs").toInt());
        group->setLastModified(query.value("lastModified").toInt());
        list->push_back(move(group));
    }
    QTextStream(stdout) << "GRP sel for ctrlr `"<< controllerId << "` returned: " << list->size() << endl;
    return list;
}

void ShadesGroupDao::clear() const
{
    QSqlQuery query(mDatabase);
    query.exec("DELETE FROM controllers_shades_groups");
}

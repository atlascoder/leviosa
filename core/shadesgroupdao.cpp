#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "shadesgroupdao.h"
#include "shadesgroup.h"

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
                                "position INTEGER"
                           ")"));
   }
}

void ShadesGroupDao::addShadesGroup(ShadesGroup& shadesGroup) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("INSERT INTO controllers_shades_groups (controllerId, name, position) VALUES (:controllerId, :name, :position)"));
    query.bindValue(":controllerId", shadesGroup.controllerId());
    query.bindValue(":name", shadesGroup.name());
    query.bindValue(":position", shadesGroup.position());
    query.exec();
    shadesGroup.setId(query.lastInsertId().toInt());
}

void ShadesGroupDao::addShadesGroupInController(int controllerId, ShadesGroup& shadesGroup) const
{
    shadesGroup.setControllerId(controllerId);
    addShadesGroup(shadesGroup);
}

void ShadesGroupDao::removeShadesGroup(int id) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM controllers_shades_groups WHERE id IS :id"));
    query.bindValue(":id", id);
    query.exec();
}

void ShadesGroupDao::updateShadesGroup(const ShadesGroup &shadesGroup) const
{
    QSqlQuery query;
    query.prepare(QString("UPDATE controllers_shades_groups SET controllerId = :controllerId, name = :name, position = :position WHERE id IS :id"));
    query.bindValue(":id", shadesGroup.id());
    query.bindValue(":controllerId", shadesGroup.controllerId());
    query.bindValue(":name", shadesGroup.name());
    query.bindValue(":position", shadesGroup.position());
    query.exec();
}

void ShadesGroupDao::removeShadesGroupsFromController(int controllerId) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM controllers_shades_groups WHERE controllerId IS :controllerId"));
    query.bindValue(":controllerId", controllerId);
    query.exec();
}

unique_ptr<vector<unique_ptr<ShadesGroup>>> ShadesGroupDao::shadesGroupsForController(int controllerId) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("SELECT ALL FROM controllers_shades_groups WHERE controllerId IS :controllerId ORDER BY position ASC"));
    query.bindValue(":controllerId", controllerId);
    unique_ptr<vector<unique_ptr<ShadesGroup>>> list(new vector<unique_ptr<ShadesGroup>>());
    while(query.next()){
        unique_ptr<ShadesGroup> group(new ShadesGroup(controllerId));
        group->setId(query.value("id").toInt());
        group->setControllerId(controllerId);
        group->setPosition(query.value("position").toInt());
        list->push_back(move(group));
    }
    return list;
}

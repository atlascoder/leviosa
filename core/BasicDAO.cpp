#include "BasicDAO.h"
#include "UserData.h"

// need to set it here due to cilick headers including otherwise
template<>
void BasicDAO<Location>::notifyDataChanged() const
{
    UserData::instance().dataChanged();
}

template<>
void BasicDAO<Controller>::notifyDataChanged() const
{
    UserData::instance().dataChanged();
}

template<>
void BasicDAO<ShadeGroup>::notifyDataChanged() const
{
    UserData::instance().dataChanged();
}

/*
 *
 *  Locations specialization
 *
 */

template<>
QString BasicDAO<Location>::tableName() { return "locations"; }

template<>
void BasicDAO<Location>::destroy(Location &item, bool notify) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE uuid=:uuid"));
    query.bindValue(":uuid", item.uuid().toString());
    query.exec();
    if(notify) notifyDataChanged();
}

template<>
QString BasicDAO<Location>::fieldsSQLDecl() const
{
    return "(" \
               "uuid TEXT PRIMARY KEY, " \
               "name TEXT, " \
               "bssid TEXT, " \
               "utcOffset INTEGER, " \
               "position INTEGER DEFAULT 0, " \
               "isSynced INTEGER DEFAULT 0" \
            ")";
}

template<>
std::unique_ptr<Location> BasicDAO<Location>::buildItem(QSqlQuery &query) const
{
    unique_ptr<Location> loc(new Location(query.value("uuid").toString()));
    loc->setName(query.value("name").toString());
    loc->setBssid(query.value("bssid").toString());
    loc->setUtcOffset(query.value("utcOffset").toInt());
    loc->setPosition(query.value("position").toInt());
    loc->setSynced(query.value("isSynced").toBool());
    return loc;
}

template<>
void BasicDAO<Location>::prepareFind(QSqlQuery &query, Location &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE uuid=:uuid"));
    query.bindValue(":uuid", item.uuid().toString());
}

template<>
void BasicDAO<Location>::prepareInsert(QSqlQuery &query, Location &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(uuid, name, bssid, utcOffset, position, isSynced) " \
                                "VALUES (:uuid, :name, :bssid, :utcOffset, :position, :isSynced)"
                            )
                        );
    query.bindValue(":uuid", item.uuid().toString());
    query.bindValue(":name", item.name());
    query.bindValue(":bssid", item.bssid());
    query.bindValue(":utcOffset", item.utcOffset());
    query.bindValue(":position", item.position());
    query.bindValue(":isSynced", item.isSynced() ? 1 : 0);
}

template<>
void BasicDAO<Location>::prepareUpdate(QSqlQuery &query, Location &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET name=:name, bssid=:bssid, utcOffset=:utcOffset, position=:position, isSynced=:isSynced WHERE uuid=:uuid"));
    query.bindValue(":uuid", item.uuid().toString());
    query.bindValue(":name", item.name());
    query.bindValue(":bssid", item.bssid());
    query.bindValue(":utcOffset", item.utcOffset());
    query.bindValue(":position", item.position());
    query.bindValue(":isSynced", item.isSynced());
}

/*
 *
 *  Controllers specialization
 *
 */

template<> QString BasicDAO<Controller>::tableName() { return "controllers"; }


template<>
void BasicDAO<Controller>::destroy(Controller &item, bool notify) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE mac=:mac"));
    query.bindValue(":mac", item.mac());
    query.exec();
    if(notify) notifyDataChanged();
}

template<>
QString BasicDAO<Controller>::fieldsSQLDecl() const
{
    return "(" \
                "mac TEXT PRIMARY KEY, " \
                "locationUuid TEXT, " \
                "name TEXT, " \
                "position INTEGER DEFAULT 0, " \
                "isSynced INTEGER DEFAULT 0" \
            ")";
}

template<>
std::unique_ptr<Controller> BasicDAO<Controller>::buildItem(QSqlQuery &query) const
{
    unique_ptr<Controller> ctrlr(new Controller);
    ctrlr->setMac(query.value("mac").toString());
    ctrlr->setLocationUuid(query.value("locationUuid").toString());
    ctrlr->setName(query.value("name").toString());
    ctrlr->setPosition(query.value("position").toInt());
    ctrlr->setSynced(query.value("isSynced").toBool());
    return ctrlr;
}

template<>
void BasicDAO<Controller>::prepareFind(QSqlQuery &query, Controller &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE mac=:mac"));
    query.bindValue(":mac", item.mac());
}

template<>
void BasicDAO<Controller>::prepareInsert(QSqlQuery &query, Controller &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(mac, locationUuid, name, position, isSynced) " \
                                "VALUES (:mac, :locationUuid, :name, :position, :isSynced)"
                            )
                        );
    query.bindValue(":mac", item.mac().toUpper());
    query.bindValue(":locationUuid", item.locationUuid());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":isSynced", item.isSynced() ? 1 : 0);
}

template<>
void BasicDAO<Controller>::prepareUpdate(QSqlQuery &query, Controller &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET locationUuid = :locationUuid, name = :name, position = :position, isSynced=:isSynced WHERE mac=:mac"));
    query.bindValue(":locationUuid", item.locationUuid());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":mac", item.mac());
    query.bindValue(":isSynced", item.isSynced() ? 1 : 0);
}

/*
 *
 *  ShadeGroups specialization
 *
 */

template<> QString BasicDAO<ShadeGroup>::tableName() { return "shadeGroups"; }

template<>
void BasicDAO<ShadeGroup>::destroy(ShadeGroup &item, bool notify) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE channel=:channel"));
    query.bindValue(":channel", item.channel());
    query.exec();
    if(notify) notifyDataChanged();
}

template<>
QString BasicDAO<ShadeGroup>::fieldsSQLDecl() const
{
    return "(" \
               "channel INTEGER PRIMARY KEY, " \
               "controllerMac TEXT NOT NULL, " \
               "name TEXT NOT NULL, " \
               "position INTEGER, " \
               "openAt INTEGER, " \
               "closeAt INTEGER, " \
               "days INTEGER DEFAULT 0, " \
               "isSynced INTEGER DEFAULT 0" \
            ")";
}

template<>
std::unique_ptr<ShadeGroup> BasicDAO<ShadeGroup>::buildItem(QSqlQuery &query) const
{
    unique_ptr<ShadeGroup> group(new ShadeGroup);
    group->setChannel((char)query.value("channel").toInt());
    group->setName(query.value("name").toString());
    group->setPosition(query.value("position").toInt());
    group->setOpenAt(query.value("openAt").toInt());
    group->setCloseAt(query.value("closeAt").toInt());
    group->setDays(query.value("days").toInt());
    group->setSynced(query.value("isSynced").toBool());
    return group;
}

template<>
void BasicDAO<ShadeGroup>::prepareFind(QSqlQuery &query, ShadeGroup &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE channel=:channel"));
    query.bindValue(":channel", item.channel());
}

template<>
void BasicDAO<ShadeGroup>::prepareInsert(QSqlQuery &query, ShadeGroup &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(channel, controllerMac, name, position, openAt, closeAt, days, isSynced) " \
                                "VALUES (:channel, :controllerMac, :name, :position, :openAt, :closeAt, :days, :isSynced)"
                            )
                        );
    query.bindValue(":channel", item.channel());
    query.bindValue(":controllerMac", item.controllerMac());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":openAt", item.openAt());
    query.bindValue(":closeAt", item.closeAt());
    query.bindValue(":days", item.days());
    query.bindValue(":isSynced", item.isSynced());
}

template<>
void BasicDAO<ShadeGroup>::prepareUpdate(QSqlQuery &query, ShadeGroup &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET controllerMac = :controllerMac, name = :name, position = :position, openAt = :openAt, closeAt = :closeAt, days = :days, isSynced=:isSynced WHERE channel=:channel"));
    query.bindValue(":channel", item.channel());
    query.bindValue(":controllerMac", item.controllerMac());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":openAt", item.openAt());
    query.bindValue(":closeAt", item.closeAt());
    query.bindValue(":days", item.days());
    query.bindValue(":isSynced", item.isSynced() ? 1 : 0);
}

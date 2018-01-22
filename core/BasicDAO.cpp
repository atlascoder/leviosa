#include "BasicDAO.h"

/*
 *
 *  Locations specialization
 *
 */

template<>
QString BasicDAO<UserLocation>::tableName() { return "locations"; }

template<>
void BasicDAO<UserLocation>::markDeleted(UserLocation &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET deleted=1 WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.exec();
}

template<>
void BasicDAO<UserLocation>::destroy(UserLocation &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.exec();
}

template<>
QString BasicDAO<UserLocation>::fieldsSQLDecl() const
{
    return "(" \
               "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
               "name TEXT, " \
               "bssid TEXT, " \
               "utcOffset INTEGER, " \
               "position INTEGER, " \
               "syncs INTEGER DEFAULT 0, " \
               "lastModified INTEGER DEFAULT 0, " \
               "deleted INTEGER DEFAULT 0" \
            ")";
}

template<>
std::unique_ptr<UserLocation> BasicDAO<UserLocation>::buildItem(QSqlQuery &query) const
{
    unique_ptr<UserLocation> loc(new UserLocation(query.value("name").toString()));
    loc->setId(query.value("id").toInt());
    loc->setBssid(query.value("bssid").toString());
    loc->setUtcOffset(query.value("utcOffset").toInt());
    loc->setPosition(query.value("position").toInt());
    loc->setSyncs(query.value("syncs").toInt());
    loc->setLastModified(query.value("lastModified").toInt());
    return loc;
}

template<>
void BasicDAO<UserLocation>::prepareFind(QSqlQuery &query, UserLocation &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE id=:id"));
    query.bindValue(":id", item.id());
}

template<>
void BasicDAO<UserLocation>::prepareInsert(QSqlQuery &query, UserLocation &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(id, name, bssid, utcOffset, position, syncs, lastModified) " \
                                "VALUES (:id, :name, :bssid, :utcOffset, :position, :syncs, :lastModified)"
                            )
                        );
    query.bindValue(":id", item.id());
    query.bindValue(":name", item.name());
    query.bindValue(":bssid", item.bssid());
    query.bindValue(":utcOffset", item.utcOffset());
    query.bindValue(":position", item.position());
    query.bindValue(":syncs", item.syncs());
    query.bindValue(":lastModified", item.lastModified());
}

template<>
void BasicDAO<UserLocation>::prepareUpdate(QSqlQuery &query, UserLocation &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET name=:name, bssid=:bssid, ssid=:ssid, utcOffset=:utcOffset, position=:position, syncs=:syncs WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.bindValue(":name", item.name());
    query.bindValue(":bssid", item.bssid());
    query.bindValue(":utcOffset", item.utcOffset());
    query.bindValue(":position", item.position());
    query.bindValue(":syncs", item.syncs());
    query.bindValue(":lastModified", item.lastModified());
}

/*
 *
 *  LocationControllers specialization
 *
 */

template<> QString BasicDAO<LocationController>::tableName() { return "controllers"; }

template<>
void BasicDAO<LocationController>::markDeleted(LocationController &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET deleted=1 WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.exec();
}

template<>
void BasicDAO<LocationController>::destroy(LocationController &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.exec();
}

template<>
QString BasicDAO<LocationController>::fieldsSQLDecl() const
{
    return "(" \
                "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
                "locationId INTEGER, " \
                "name TEXT, " \
                "mac TEXT, " \
                "position INTEGER, " \
                "syncs INTEGER DEFAULT 0, " \
                "lastModified INTEGER DEFAULT 0" \
            ")";
}

template<>
std::unique_ptr<LocationController> BasicDAO<LocationController>::buildItem(QSqlQuery &query) const
{
    unique_ptr<LocationController> ctrlr(new LocationController(query.value("locationId").toInt()));
    ctrlr->setName(query.value("name").toString());
    ctrlr->setMac(query.value("mac").toString());
    ctrlr->setPosition(query.value("position").toInt());
    ctrlr->setSyncs(query.value("syncs").toInt());
    ctrlr->setLastModified(query.value("lastModified").toInt());
    return ctrlr;
}

template<>
void BasicDAO<LocationController>::prepareFind(QSqlQuery &query, LocationController &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE id=:id"));
    query.bindValue(":id", item.id());
}

template<>
void BasicDAO<LocationController>::prepareInsert(QSqlQuery &query, LocationController &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(locationId, name, mac, position, syncs, lastModified) " \
                                "VALUES (:locationId, :name, :mac, :position, :syncs, :lastModified)"
                            )
                        );
    query.bindValue(":locationId", item.locationId());
    query.bindValue(":name", item.name());
    query.bindValue(":mac", item.mac().toUpper());
    query.bindValue(":position", item.position());
    query.bindValue(":syncs", item.syncs());
    query.bindValue(":lastModified", item.lastModified());
}

template<>
void BasicDAO<LocationController>::prepareUpdate(QSqlQuery &query, LocationController &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET locationId = :locationId, name = :name, mac = :mac, position = :position, syncs=:syncs, lastModified=:lastModified WHERE id=:id"));
    query.bindValue(":locationId", item.locationId());
    query.bindValue(":name", item.name());
    query.bindValue(":mac", item.mac().toUpper());
    query.bindValue(":position", item.position());
    query.bindValue(":id", item.id());
    query.bindValue(":syncs", item.syncs());
    query.bindValue(":lastModified", item.lastModified());
}

/*
 *
 *  ShadeGroups specialization
 *
 */

template<> QString BasicDAO<ShadeGroup>::tableName() { return "shadeGroups"; }

template<>
void BasicDAO<ShadeGroup>::markDeleted(ShadeGroup &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET deleted=1 WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.exec();
}

template<>
void BasicDAO<ShadeGroup>::destroy(ShadeGroup &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.exec();
}

template<>
QString BasicDAO<ShadeGroup>::fieldsSQLDecl() const
{
    return "(" \
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
            ")";
}

template<>
std::unique_ptr<ShadeGroup> BasicDAO<ShadeGroup>::buildItem(QSqlQuery &query) const
{
    unique_ptr<ShadeGroup> group(new ShadeGroup(query.value("controllerId").toInt()));
    group->setId(query.value("id").toInt());
    group->setName(query.value("name").toString());
    group->setPosition(query.value("position").toInt());
    group->setOpenAt(query.value("openAt").toInt());
    group->setCloseAt(query.value("closeAt").toInt());
    group->setDays(query.value("days").toInt());
    group->setChannel((char)query.value("channel").toInt());
    group->setSyncs(query.value("syncs").toInt());
    group->setLastModified(query.value("lastModified").toInt());
    return group;
}

template<>
void BasicDAO<ShadeGroup>::prepareFind(QSqlQuery &query, ShadeGroup &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE id=:id"));
    query.bindValue(":id", item.id());
}

template<>
void BasicDAO<ShadeGroup>::prepareInsert(QSqlQuery &query, ShadeGroup &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(controllerId, name, position, openAt, closeAt, days, channel, syncs, lastModified) " \
                                "VALUES (:controllerId, :name, :position, :openAt, :closeAt, :days, :channel, :syncs, :lastModified)"
                            )
                        );
    query.bindValue(":controllerId", item.controllerId());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":openAt", item.openAt());
    query.bindValue(":closeAt", item.closeAt());
    query.bindValue(":days", item.days());
    query.bindValue(":channel", item.channel());
    query.bindValue(":syncs", item.syncs());
    query.bindValue(":lastModified", item.lastModified());
}

template<>
void BasicDAO<ShadeGroup>::prepareUpdate(QSqlQuery &query, ShadeGroup &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET controllerId = :controllerId, name = :name, position = :position, openAt = :openAt, closeAt = :closeAt, days = :days, channel = :channel, syncs=:syncs, lastModified=:lastModified WHERE id=:id"));
    query.bindValue(":id", item.id());
    query.bindValue(":controllerId", item.controllerId());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":openAt", item.openAt());
    query.bindValue(":closeAt", item.closeAt());
    query.bindValue(":days", item.days());
    query.bindValue(":channel", item.channel());
    query.bindValue(":syncs", item.syncs());
    query.bindValue(":lastModified", item.lastModified());
}

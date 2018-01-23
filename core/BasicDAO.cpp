#include "BasicDAO.h"
#include "UserData.h"

// need to set it here due to cilick headers including otherwise
template<>
void BasicDAO<UserLocation>::notifyDataChanged() const
{
    UserData::instance().dataChanged();
}

template<>
void BasicDAO<LocationController>::notifyDataChanged() const
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
QString BasicDAO<UserLocation>::tableName() { return "locations"; }

template<>
void BasicDAO<UserLocation>::destroy(UserLocation &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE uuid=:uuid"));
    query.bindValue(":uuid", item.uuid().toString());
    query.exec();
    notifyDataChanged();
}

template<>
QString BasicDAO<UserLocation>::fieldsSQLDecl() const
{
    return "(" \
               "uuid TEXT PRIMARY KEY, " \
               "name TEXT, " \
               "bssid TEXT, " \
               "utcOffset INTEGER, " \
               "position INTEGER, " \
               "syncs INTEGER DEFAULT 0, " \
               "lastModified INTEGER DEFAULT 0" \
            ")";
}

template<>
std::unique_ptr<UserLocation> BasicDAO<UserLocation>::buildItem(QSqlQuery &query) const
{
    unique_ptr<UserLocation> loc(new UserLocation(query.value("uuid").toString()));
    loc->setName(query.value("name").toString());
    loc->setBssid(query.value("bssid").toString());
    loc->setUtcOffset(query.value("utcOffset").toInt());
    loc->setPosition(query.value("position").toInt());
    loc->setLastModified(query.value("lastModified").toInt());
    return loc;
}

template<>
void BasicDAO<UserLocation>::prepareFind(QSqlQuery &query, UserLocation &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE uuid=:uuid"));
    query.bindValue(":uuid", item.uuid().toString());
}

template<>
void BasicDAO<UserLocation>::prepareInsert(QSqlQuery &query, UserLocation &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(uuid, name, bssid, utcOffset, position, lastModified) " \
                                "VALUES (:uuid, :name, :bssid, :utcOffset, :position, :lastModified)"
                            )
                        );
    query.bindValue(":uuid", item.uuid().toString());
    query.bindValue(":name", item.name());
    query.bindValue(":bssid", item.bssid());
    query.bindValue(":utcOffset", item.utcOffset());
    query.bindValue(":position", item.position());
    query.bindValue(":lastModified", item.lastModified());
}

template<>
void BasicDAO<UserLocation>::prepareUpdate(QSqlQuery &query, UserLocation &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET name=:name, bssid=:bssid, utcOffset=:utcOffset, position=:position, lastModified=:lastModified WHERE uuid=:uuid"));
    query.bindValue(":uuid", item.uuid().toString());
    query.bindValue(":name", item.name());
    query.bindValue(":bssid", item.bssid());
    query.bindValue(":utcOffset", item.utcOffset());
    query.bindValue(":position", item.position());
    query.bindValue(":lastModified", item.lastModified());
}

/*
 *
 *  LocationControllers specialization
 *
 */

template<> QString BasicDAO<LocationController>::tableName() { return "controllers"; }


template<>
void BasicDAO<LocationController>::destroy(LocationController &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE mac=:mac"));
    query.bindValue(":mac", item.mac());
    query.exec();
    notifyDataChanged();
}

template<>
QString BasicDAO<LocationController>::fieldsSQLDecl() const
{
    return "(" \
                "mac TEXT PRIMARY KEY, " \
                "locationUuid TEXT, " \
                "name TEXT, " \
                "position INTEGER DEFAULT 0 " \
            ")";
}

template<>
std::unique_ptr<LocationController> BasicDAO<LocationController>::buildItem(QSqlQuery &query) const
{
    unique_ptr<LocationController> ctrlr(new LocationController);
    ctrlr->setMac(query.value("mac").toString());
    ctrlr->setMac(query.value("locationUuid").toString());
    ctrlr->setName(query.value("name").toString());
    ctrlr->setPosition(query.value("position").toInt());
    return ctrlr;
}

template<>
void BasicDAO<LocationController>::prepareFind(QSqlQuery &query, LocationController &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE mac=:mac"));
    query.bindValue(":mac", item.mac());
}

template<>
void BasicDAO<LocationController>::prepareInsert(QSqlQuery &query, LocationController &item) const
{
    query.prepare(QString("INSERT INTO ").append(tableName())
                        .append(
                                "(mac, locationUuid, name, position) " \
                                "VALUES (:mac, :locationUuid, :name, :position)"
                            )
                        );
    query.bindValue(":mac", item.mac().toUpper());
    query.bindValue(":locationUuid", item.locationUuid());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
}

template<>
void BasicDAO<LocationController>::prepareUpdate(QSqlQuery &query, LocationController &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET locationUuid = :locationUuid, name = :name, position = :position WHERE mac=:mac"));
    query.bindValue(":locationUuid", item.locationUuid());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":mac", item.mac());
}

/*
 *
 *  ShadeGroups specialization
 *
 */

template<> QString BasicDAO<ShadeGroup>::tableName() { return "shadeGroups"; }

template<>
void BasicDAO<ShadeGroup>::destroy(ShadeGroup &item) const
{
    QSqlQuery query(mDatabase);
    query.prepare(QString("DELETE FROM ").append(tableName()).append(" WHERE channel=:channel"));
    query.bindValue(":channel", item.channel());
    query.exec();
    notifyDataChanged();
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
               "days INTEGER DEFAULT 0" \
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
                                "(channel, controllerMac, name, position, openAt, closeAt, days) " \
                                "VALUES (:channel, :controllerMac, :name, :position, :openAt, :closeAt, :days)"
                            )
                        );
    query.bindValue(":channel", item.channel());
    query.bindValue(":controllerMac", item.controllerMac());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":openAt", item.openAt());
    query.bindValue(":closeAt", item.closeAt());
    query.bindValue(":days", item.days());
}

template<>
void BasicDAO<ShadeGroup>::prepareUpdate(QSqlQuery &query, ShadeGroup &item) const
{
    query.prepare(QString("UPDATE ").append(tableName()).append(" SET controllerMac = :controllerMac, name = :name, position = :position, openAt = :openAt, closeAt = :closeAt, days = :days WHERE channel=:channel"));
    query.bindValue(":channel", item.channel());
    query.bindValue(":controllerMac", item.controllerMac());
    query.bindValue(":name", item.name());
    query.bindValue(":position", item.position());
    query.bindValue(":openAt", item.openAt());
    query.bindValue(":closeAt", item.closeAt());
    query.bindValue(":days", item.days());
}

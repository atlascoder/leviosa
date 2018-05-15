#include "CognitoSyncDAO.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#define TABLE_NAME "cognito_sync"

void CognitoSyncDAO::init() const
{
    if(!mDatabase.tables().contains(TABLE_NAME)){
        QSqlQuery query(mDatabase);
        if (query.exec(
                    "CREATE TABLE " \
                    TABLE_NAME \
                    "(" \
                    "key TEXT PRIMARY KEY NOT NULL, " \
                    "value TEXT, " \
                    "syncCount INTEGER DEFAULT 0, " \
                    "deleted INTEGER DEFAULT 0, " \
                    "modified INTEGER DEFAULT 0 " \
                    ")"
                ))
            qDebug() << "Table " << TABLE_NAME << " was created";
        else
            qDebug() << "Table " << TABLE_NAME << " not created: " << query.lastError().text();
    }
}

void CognitoSyncDAO::clear() const
{
    QSqlQuery query(mDatabase);
    if (query.exec("DELETE FROM "  TABLE_NAME)
    )
        qDebug() << "Table " << TABLE_NAME << " cleared";
    else
        qDebug() << "Table " << TABLE_NAME << " not cleared: " << query.lastError().text();
}

long long CognitoSyncDAO::lastSyncCount() const
{
    QSqlQuery query(mDatabase);
    if (query.exec(
                "SELECT MAX(syncCount) AS lastSyncCount FROM " \
                TABLE_NAME
                )
    ) {
        if (query.first())
            return query.value("lastSyncCount").toLongLong();
    }
    else {
        qDebug() << "Getting lastSyncCount error: " << query.lastError().text();
    }
    return 0;
}

shared_ptr<vector<shared_ptr<Syncable>>> CognitoSyncDAO::select(const QString &condition) const
{
    shared_ptr<vector<shared_ptr<Syncable>>> result(new vector<shared_ptr<Syncable>>);
    QSqlQuery query(mDatabase);
    if (condition.isEmpty())
        query.prepare(
                    "SELECT * FROM " \
                    TABLE_NAME
                );
    else
        query.prepare(
                    "SELECT * FROM " \
                    TABLE_NAME \
                    " WHERE " + condition
                );
    if (query.exec()) {
        while (query.next()) {
            shared_ptr<Syncable> item(new Syncable(query.value("key").toString()));
            item->setSyncCount(query.value("syncCount").toLongLong());
            item->setChanged(query.value("modified").toInt() != 0);
            item->setValue(query.value("value").toString());
            item->setDeleted(query.value("deleted").toInt() != 0);
            result->push_back(item);
        }
    }
    else
        qDebug() << "Error requesting syncables: " << query.lastError().text();
    return result;
}

shared_ptr<vector<shared_ptr<Syncable>>> CognitoSyncDAO::items() const
{
    return select();
}


shared_ptr<vector<shared_ptr<Syncable>>> CognitoSyncDAO::modified() const
{
    return select("modified IS TRUE");
}

void CognitoSyncDAO::persistChanged(const shared_ptr<vector<shared_ptr<Syncable>>> &items) const
{
    persistItems(items, true);
}

void CognitoSyncDAO::persistItems(const shared_ptr<vector<shared_ptr<Syncable>>> &items, bool changedOnly) const
{
    shared_ptr<vector<shared_ptr<Syncable>>> current = CognitoSyncDAO::items();
    bool transaction_prepared = false;
    bool transaction = mDatabase.transaction();
    for (auto& i : *items) {
        if (changedOnly && !i->isChanged()) continue;

        auto f = find_if(current->begin(), current->end(), [i](const shared_ptr<Syncable>& _i)->bool{ return i->uuid()==_i->uuid(); });
        if (f == current->end()) {
            QSqlQuery query(mDatabase);
            query.prepare(
                        "INSERT INTO " \
                        TABLE_NAME \
                        " (key, value, syncCount, deleted, modified) VALUES (:key, :value, :syncCount, :deleted, :modified)"
            );
            query.bindValue(":key", i->uuid());
            query.bindValue(":value", i->value());
            query.bindValue(":syncCount", i->syncCount());
            query.bindValue(":deleted", i->isDeleted());
            query.bindValue(":modified", i->isChanged());
            if (query.exec() && !transaction_prepared) {
                transaction_prepared = true;
            }
            else if (!transaction_prepared) {
                qDebug() << "Error while executing SQL: " << query.lastError().text();
                if (transaction) {
                    transaction_prepared = false;
                    break;
                }
            }
        }
        else {
            QSqlQuery query(mDatabase);
            query.prepare(
                        "UPDATE " \
                        TABLE_NAME \
                        " SET value=:value, syncCount=:syncCount, deleted=:deleted, modified=:modified WHERE key=:key"
            );
            query.bindValue(":key", i->uuid());
            query.bindValue(":value", i->value());
            query.bindValue(":syncCount", i->syncCount());
            query.bindValue(":deleted", i->isDeleted() ? 1 : 0);
            query.bindValue(":modified", i->isChanged() ? 1 : 0);
            if (query.exec() && !transaction_prepared) {
                transaction_prepared = true;
            }
            else if (!transaction_prepared) {
                qDebug() << "Error while executing SQL: " << query.lastError().text();
                if (transaction) {
                    transaction_prepared = false;
                    break;
                }
            }
        }
    }
    if (transaction) {
        if (!transaction_prepared) {
            qDebug() << "Transaction was not prepared, rollback";
            mDatabase.rollback();
        }
        else if (mDatabase.commit()) {
            qDebug() << "Data persisted to table `" << TABLE_NAME << "` in transaction mode";
        }
        else {
            qDebug() << "Data not persisted to table `" << TABLE_NAME << "` with transaction due error: " << mDatabase.lastError().text();
        }
    }
}

void CognitoSyncDAO::persistSynced(const shared_ptr<vector<shared_ptr<Syncable>>> &items) const
{
    shared_ptr<vector<shared_ptr<Syncable>>> current = CognitoSyncDAO::items();
    bool transaction_prepared = false;
    bool transaction = mDatabase.transaction();
    for (auto& i : *items) {
        auto f = find_if(current->begin(), current->end(), [i](const shared_ptr<Syncable>& _i)->bool{ return i->uuid()==_i->uuid(); });
        if (f == current->end()) {
            QSqlQuery query(mDatabase);
            query.prepare(
                        "INSERT INTO " \
                        TABLE_NAME \
                        " (key, value, syncCount, deleted, modified) VALUES (:key, :value, :syncCount, :deleted, :modified)"
            );
            query.bindValue(":key", i->uuid());
            query.bindValue(":value", i->value());
            query.bindValue(":syncCount", i->syncCount());
            query.bindValue(":deleted", i->isDeleted());
            query.bindValue(":modified", i->isChanged());
            if (query.exec()) {
                transaction_prepared = true;
            }
            else {
                qDebug() << "Error while executing SQL: " << query.lastError().text();
                if (transaction) {
                    transaction_prepared = false;
                    break;
                }
            }
        }
        else {
            QSqlQuery query(mDatabase);
            query.prepare(
                        "UPDATE " \
                        TABLE_NAME \
                        " SET value=:value, syncCount=:syncCount, deleted=:deleted, modified=:modified WHERE key=:key"
            );
            query.bindValue(":key", i->uuid());
            query.bindValue(":value", i->value());
            query.bindValue(":syncCount", i->syncCount());
            query.bindValue(":deleted", i->isDeleted() ? 1 : 0);
            query.bindValue(":modified", i->isChanged() ? 1 : 0);
            if (query.exec()) {
                transaction_prepared = true;
            }
            else {
                qDebug() << "Error while executing SQL: " << query.lastError().text();
                if (transaction) {
                    transaction_prepared = false;
                    break;
                }
            }
        }
    }
    if (transaction) {
        if (!transaction_prepared) {
            qDebug() << "Transaction was not prepared, rollback.";
            mDatabase.rollback();
        }
        else if (mDatabase.commit()) {
            qDebug() << "Data persisted to table `" << TABLE_NAME << "` in transaction mode";
        }
        else {
            qDebug() << "Transaction commit to table `" << TABLE_NAME << "` failed due error: " << mDatabase.lastError().text();
        }
    }

}

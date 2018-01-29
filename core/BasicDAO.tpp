#include "BasicDAO.h"
#include "location.h"
#include "controller.h"
#include "shadegroup.h"

#include <QSqlError>
#include <QDebug>


// User location concretization
#include "core/location.h"

using namespace std;

// Generic behavior

template<class T>
void BasicDAO<T>::init() const
{
    if(!mDatabase.tables().contains(tableName())){
        QSqlQuery query(mDatabase);
        query.exec(QString("CREATE TABLE ").append(tableName()).append(fieldsSQLDecl()));
        if(query.lastError().isValid())
            qDebug() << "DAO Table " << tableName() << " not created: " << query.lastError().text();
        else
            qDebug() << "DAO Table " << tableName() << " created.";
    }
    else{
        qDebug() << "DAO Table " << tableName() << " already exist.";
    }
}

template<class T>
unique_ptr<vector<unique_ptr<T>>> BasicDAO<T>::items() const
{
    QSqlQuery query(mDatabase);
    QString selectSQL = QString("SELECT * FROM ").append(tableName()).append(" ORDER BY position ASC");
    query.exec(selectSQL);
    if(query.lastError().isValid())
        QTextStream(stdout) << "error: " << query.lastError().text() << " // SQL: " << selectSQL;
    unique_ptr<vector<unique_ptr<T>>> list(new vector<unique_ptr<T>>());
    while(query.next()){
        unique_ptr<T> loc = buildItem(query);
        list->push_back(move(loc));
    }
    return list;
}

template<class T>
void BasicDAO<T>::loadItems(std::vector<std::unique_ptr<T>>& receiver) const
{
    QSqlQuery query(mDatabase);
    QString selectSQL = QString("SELECT * FROM ").append(tableName()).append(" ORDER BY position ASC");
    query.exec(selectSQL);
    if(query.lastError().isValid())
        QTextStream(stdout) << "error: " << query.lastError().text() << " // SQL: " << selectSQL;
    while(query.next()){
        unique_ptr<T> loc = buildItem(query);
        receiver.push_back(move(loc));
    }
}

template<class T>
void BasicDAO<T>::persistItem(T &item, bool notify) const
{
    QSqlQuery query(mDatabase);
    prepareFind(query, item);
    query.exec();
    if(query.lastError().isValid()){
        qDebug() << "DAO find " << tableName() << " failed: " << query.lastError().text() << " // SQL: " << query.executedQuery();
        return;
    }
    else if(query.first())
        prepareUpdate(query, item);
    else
        prepareInsert(query, item);

    query.exec();
    if(query.lastError().isValid()){
        qDebug() << "DAO persist " << tableName() << " failed: " << query.lastError().text() << " // SQL: " << query.executedQuery();
    }
    else{
        qDebug() << "DAO persist " << tableName() << " completed.";
        if(notify) notifyDataChanged();
    }
}

template<class T>
void BasicDAO<T>::clear(bool notify) const
{
    QSqlQuery query(mDatabase);
    query.exec(QString("DELETE FROM ").append(tableName()));
    if(notify) notifyDataChanged();
}

template<class T>
int BasicDAO<T>::lastModified() const
{
    QSqlQuery query(mDatabase);
    query.exec(QString("SELECT MAX(lastModified) AS latestModified FROM ").append(tableName()));

    if(query.first())
        return query.value("latestModified").toInt();
    else
        return 0;
}

template<class T>
unique_ptr<vector<unique_ptr<T>>> BasicDAO<T>::filtered(const QString& field, const QVariant & filter) const
{
    QSqlQuery query(mDatabase);
    QString selectSQL = QString("SELECT * FROM ").append(tableName()).append(" WHERE ").append(field).append("=:filter ORDER BY position ASC");
    query.prepare(selectSQL);
    query.bindValue(":filter", filter);
    query.exec();
    if(query.lastError().isValid())
        QTextStream(stdout) << "error: " << query.lastError().text() << " // SQL: " << selectSQL;
    unique_ptr<vector<unique_ptr<T>>> list(new vector<unique_ptr<T>>());
    while(query.next()){
        unique_ptr<T> loc = buildItem(query);
        list->push_back(move(loc));
    }
    return list;
}

template<class T>
void BasicDAO<T>::loadFiltered(std::vector<std::unique_ptr<T> > &receiver, const QString &field, const QVariant &filter) const
{
    QSqlQuery query(mDatabase);
    QString selectSQL = QString("SELECT * FROM ").append(tableName()).append(" WHERE ").append(field).append("=:filter ORDER BY position ASC");
    query.prepare(selectSQL);
    query.bindValue(":filter", filter);
    query.exec();
    if(query.lastError().isValid())
        QTextStream(stdout) << "error: " << query.lastError().text() << " // SQL: " << selectSQL;
    while(query.next()){
        unique_ptr<T> loc = buildItem(query);
        receiver.push_back(move(loc));
    }
}

template<class T>
bool BasicDAO<T>::isSynced() const
{
    QSqlQuery query(mDatabase);
    QString sql = QString("SELECT COUNT(*) AS notSynced FROM ").append(tableName()).append(" WHERE isSynced=0");
    if(query.exec(sql) && query.first()){
        return !query.value("notSynced").toBool();
    }
    else{
        qDebug() << "isSynced failed: " << query.lastError().text() << " // SQL: " << query.lastQuery();
        return false;
    }
}

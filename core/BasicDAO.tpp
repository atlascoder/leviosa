#include "BasicDAO.h"
#include "Location.h"
#include "Controller.h"
#include "ShadeGroup.h"

#include <QSqlError>
#include <QDebug>


// User location concretization
#include "core/Location.h"

using namespace std;

// Generic behavior

template<class T>
void BasicDAO<T>::init() const
{
    if(!mDatabase.tables().contains(tableName())){
        QSqlQuery query(mDatabase);
        query.exec("CREATE TABLE " + tableName() + fieldsSQLDecl());
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
unique_ptr<vector<unique_ptr<T>>> BasicDAO<T>::items(bool excludeDeleted) const
{
    QSqlQuery query(mDatabase);
    
    QString selectSQL("SELECT * FROM " + tableName());
    if (excludeDeleted)  selectSQL += " WHERE deleted IS 0";
    selectSQL += " ORDER BY position ASC";
    
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
    QString selectSQL = "SELECT * FROM " + tableName() + " WHERE deleted IS 0 ORDER BY position ASC";
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
unique_ptr<vector<unique_ptr<T>>> BasicDAO<T>::filtered(const QString& field, const QVariant & filter) const
{
    QSqlQuery query(mDatabase);
    QString selectSQL("SELECT * FROM " + tableName() + " WHERE " + field + "=:filter AND deleted IS 0 ORDER BY position ASC");
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
    QString selectSQL("SELECT * FROM " + tableName() + " WHERE " + field + "=:filter WHERE deleted IS 0 ORDER BY position ASC");
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
bool BasicDAO<T>::isChanged() const
{
    QSqlQuery query(mDatabase);
    QString sql("SELECT COUNT(*) AS changedCount FROM " + tableName() + " WHERE changed IS NOT 0 AND deleted IS 0");
    if(query.exec(sql) && query.first()){
        return query.value("changedCount").toInt() > 0;
    }
    else{
        qDebug() << "isSynced failed: " << query.lastError().text() << " // SQL: " << query.lastQuery();
        return false;
    }
}

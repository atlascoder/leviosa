#include "BasicDAO.h"
#include "userlocation.h"
#include "locationcontroller.h"
#include "shadegroup.h"

#include <QSqlError>
#include <QDebug>


// User location concretization
#include "core/userlocation.h"

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
    QString selectSQL = QString("SELECT * FROM ").append(tableName()).append(" WHERE deleted IS 0 ORDER BY position ASC");
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
void BasicDAO<T>::persistItem(T &item) const
{
    QSqlQuery query(mDatabase);
    prepareFind(query, item);
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
        item.setId(query.value("id").toInt());
        qDebug() << "DAO persist " << tableName() << " completed.";
    }
}

template<class T>
void BasicDAO<T>::clear() const
{
    QSqlQuery query(mDatabase);
    query.exec(QString("DELETE FROM ").append(tableName()));
}

template<class T>
void BasicDAO<T>::prepareFind(QSqlQuery &query, T &item) const
{
    query.prepare(QString("SELECT * FROM ").append(tableName()).append(" WHERE id IS :id"));
    query.bindValue(":id", item.id());
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

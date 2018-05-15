#include "CredentialsDAO.h"
#include <QSqlDatabase>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

CredentialsDAO::CredentialsDAO(QSqlDatabase & database) : mDatabase(database)
{

}

void CredentialsDAO::init() const
{
    if(!mDatabase.tables().contains("credentials")){
        QSqlQuery query(mDatabase);
        query.exec(
                    "CREATE TABLE credentials (" \
                    "login TEXT PRIMARY KEY NOT NULL, " \
                    "accessToken TEXT, " \
                    "longtermToken TEXT, " \
                    "longtermTokenExpiration INTEGER" \
                    ")"
                   );
    }
}

void CredentialsDAO::persistLogins(const QList<UserLogin>& logins) const
{
    QSqlQuery query(mDatabase);
    query.exec("DELETE FROM credentials");
    for (UserLogin l: logins) {
        query.prepare(
                    "INSERT INTO credentials (login, accessToken, longtermToken, longtermTokenExpiration) " \
                    " VALUES (:login, :accessToken, :longtermToken, :longtermTokenExpiration)" \
            );
        query.bindValue(":login", l.login);
        query.bindValue(":accessToken", l.accessToken);
        query.bindValue(":longtermToken", l.longtermToken);
        query.bindValue(":longtermTokenExpiration", l.longtermExpiration);
        if(query.exec())
            qDebug() << "login " << l.login << " persisted";
        else
            qDebug() << "persist login " << l.login << " failed: " << query.lastError().text();
    }
}

QList<UserLogin> CredentialsDAO::readLogins() const
{
    QList<UserLogin> result;
    QSqlQuery query(mDatabase);
    query.prepare("SELECT * FROM credentials");
    if (query.exec()) {
        while(query.next()) {
            UserLogin login;
            login.login = query.value("login").toString();
            login.accessToken = query.value("accessToken").toString();
            login.longtermToken = query.value("longtermToken").toString();
            login.longtermExpiration = query.value("longtermTokenExpiration").toLongLong();
            result.append(login);
        }
    }
    else {
        qDebug() << "Identity presist read failed: " << query.lastError().text();
    }
    return result;
}

void CredentialsDAO::clear() const
{
    QSqlQuery query;
    if (query.exec("DELETE FROM credentials"))
        qDebug() << "Table `credentials` cleared";
    else
        qDebug() << "Table `credentials` not cleared";
}

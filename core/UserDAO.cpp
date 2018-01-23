#include "UserDAO.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

UserDAO::UserDAO(QSqlDatabase & database) : mDatabase(database)
{

}

void UserDAO::init() const
{
    if(!mDatabase.tables().contains("users")){
        QSqlQuery query(mDatabase);
        query.exec(
                    "CREATE TABLE users (" \
                    "email TEXT PRIMARY KEY NOT NULL, " \
                    "idToken TEXT, " \
                    "idTokenExpiration INTEGER DEFAULT 0, " \
                    "accessToken TEXT, " \
                    "accessTokenExpiration INTEGER DEFAULT 0, " \
                    "refreshToken TEXT, " \
                    "refreshTokenExpiration INTEGER DEFAULT 0, " \
                    "locationsModified INTEGER DEFAULT 0, " \
                    "controllersModified INTEGER DEFAULT 0, " \
                    "shadeGroupsModified INTEGER DEFAULT 0"
                    ")"
                   );
    }
}

void UserDAO::createUser(User &user) const
{
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO users (email, idToken, idTokenExpiration, accessToken, accessTokenExpiration, refreshToken, refreshTokenExpiration, locationsModified, controllersModified, shadeGroupsModified)" \
                  " VALUES (:email, :idToken, :idTokenExpiration, :accessToken, :accessTokenExpiration, :refreshToken, :refreshTokenExpiration, :locationsModified, :controllersModified, :shadeGroupsModified)"
                  );
    query.bindValue(":email", user.email());
    query.bindValue(":idToken", user.idToken());
    query.bindValue(":idTokenExpiration", user.idTokenExpiration());
    query.bindValue(":accessToken", user.accessToken());
    query.bindValue(":accessTokenExpiration", user.accessTokenExpiration());
    query.bindValue(":refreshToken", user.refreshToken());
    query.bindValue(":refreshTokenExpiration", user.refreshTokenExpiration());
    query.bindValue(":locationsModified", user.locationsModified());
    query.bindValue(":controllersModified", user.controllersModified());
    query.bindValue(":shadeGroupsModified", user.shadeGroupsLastModified());
    query.exec();
    if(query.lastError().isValid()){
        qDebug() << "User " << user.email() << " not created: " << query.lastError().text() << "\n" << \
                    " // SQL: " << query.lastQuery();
    }
}

void UserDAO::updateUser(const User &user) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE users SET idToken=:idToken, idTokenExpiration=:idTokenExpiration, accessToken=:accessToken, accessTokenExpiration=:accessTokenExpiration, refreshToken=:refreshToken, refreshTokenExpiration=:refreshTokenExpiration, locationsModified=:locationsModified, controllersModified=:controllersModified, shadeGroupsModified=:shadeGroupsModified WHERE email=:email");
    query.bindValue(":idToken", user.idToken());
    query.bindValue(":idTokenExpiration", user.idTokenExpiration());
    query.bindValue(":accessToken", user.idToken());
    query.bindValue(":accessTokenExpiration", user.accessTokenExpiration());
    query.bindValue(":refreshToken", user.idToken());
    query.bindValue(":refreshTokenExpiration", user.refreshTokenExpiration());
    query.bindValue(":locationsModified", user.locationsModified());
    query.bindValue(":controllersModified", user.controllersModified());
    query.bindValue(":shadeGroupsModified", user.shadeGroupsLastModified());
    query.bindValue(":email", user.email());
    query.exec();
    if(query.lastError().isValid())
        qDebug() << "Updating user " << user.email() << "failed: " << query.lastError().text() << "\n" << \
                    " // SQL: " << query.lastQuery();
}

bool UserDAO::loadUser(User &user) const
{
    QSqlQuery query(mDatabase);
    query.exec("SELECT * FROM users");
    if(query.lastError().isValid()){
        qDebug() << "Loading User error:" << query.lastError().text() << endl << "\n" << \
                    " // SQL: " << query.lastQuery();
        return false;
    }
    else if(query.first()){
        user.setEmail(query.value("email").toString());
        user.setIdToken(query.value("idToken").toString());
        user.setIdTokenExpiration(query.value("idTokenExpiration").toInt());
        user.setAccessToken(query.value("accessToken").toString());
        user.setAccessTokenExpiration(query.value("accessTokenExpiration").toInt());
        user.setRefreshToken(query.value("refreshToken").toString());
        user.setRefreshTokenExpiration(query.value("refreshTokenExpiration").toInt());
        user.setLocationsModified(query.value("locationsModified").toInt());
        user.setControllersModified(query.value("controllersModified").toInt());
        user.setShadeGroupsModified(query.value("shadeGroupsModified").toInt());
        return true;
    }
    else{
        qDebug() << "No users in database." << endl;
        return false;
    }
}

void UserDAO::clear() const
{
    QSqlQuery query(mDatabase);
    query.exec("DELETE FROM users");
}

void UserDAO::persistUser(User &user) const
{
    User currentUser;
    if(loadUser(currentUser)){
        if(user.email() == currentUser.email())
            updateUser(user);
        else{
            clear();
            createUser(user);
        }
    }
    else{
        createUser(user);
    }
}

void UserDAO::persistUserDataModified(const User& user) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE users SET locationsModified=:lm, controllersModified=:cm, shadeGroupsModified=:gm WHERE email=:email");
    query.bindValue(":lm", user.locationsModified());
    query.bindValue(":cm", user.controllersModified());
    query.bindValue(":gm", user.shadeGroupsLastModified());
    query.bindValue(":email", user.email());
    if(query.exec()){
        qDebug() << "UserData modified persisted";
    }
    else{
        qDebug() << "UserData modified persisting error: " << query.lastError().text() << " // SQL: " << query.lastQuery();
    }
}

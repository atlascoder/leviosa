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
                    "shadeGroupsModified INTEGER DEFAULT 0, " \
                    "locationsSyncCount INTEGER DEFAULT 0, " \
                    "controllersSyncCount INTEGER DEFAULT 0, " \
                    "shadeGroupsSyncCount INTEGER DEFAULT 0, " \
                    "locationsSynced INTEGER DEFAULT 0, " \
                    "controllersSynced INTEGER DEFAULT 0, " \
                    "shadeGroupsSynced INTEGER DEFAULT 0, " \
                    "verified INTEGER DEFAULT 0" \
                    ")"
                   );
    }
}

void UserDAO::createUser(User &user) const
{
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO users (email, idToken, idTokenExpiration, accessToken, accessTokenExpiration, refreshToken, refreshTokenExpiration, locationsModified, controllersModified, shadeGroupsModified, locationsSyncCount, controllersSyncCount, shadeGroupsSyncCount, locationsSynced, controllersSynced, shadeGroupsSynced, verified)" \
                  " VALUES (:email, :idToken, :idTokenExpiration, :accessToken, :accessTokenExpiration, :refreshToken, :refreshTokenExpiration, :locationsModified, :controllersModified, :shadeGroupsModified, :locationsSyncCount, :controllersSyncCount, :shadeGroupsSyncCount, :locationsSynced, :controllersSynced, :shadeGroupsSynced, :verified)"
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
    query.bindValue(":locationsSyncCount", user.locationsSyncCount());
    query.bindValue(":controllersSyncCount", user.controllersSyncCount());
    query.bindValue(":shadeGroupsSyncCount", user.shadeGroupsSyncCount());
    query.bindValue(":locationsSynced", user.locationsSynced());
    query.bindValue(":controllersSynced", user.controllersSynced());
    query.bindValue(":shadeGroupsSynced", user.shadeGroupsSynced());
    query.bindValue(":verified", user.verified() ? 1 : 0);
    query.exec();
    if(query.lastError().isValid()){
        qDebug() << "User " << user.email() << " not created: " << query.lastError().text() << "\n" << \
                    " // SQL: " << query.lastQuery();
    }
}

void UserDAO::updateUser(const User &user) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE users SET idToken=:idToken, idTokenExpiration=:idTokenExpiration, accessToken=:accessToken, accessTokenExpiration=:accessTokenExpiration, refreshToken=:refreshToken, refreshTokenExpiration=:refreshTokenExpiration, locationsModified=:locationsModified, controllersModified=:controllersModified, shadeGroupsModified=:shadeGroupsModified, locationsSyncCount=:locationsSyncCount, controllersSyncCount=:controllersSyncCount, shadeGroupsSyncCount=:shadeGroupsSyncCount, locationsSynced=:locationsSynced, controllersSynced=:controllersSynced, shadeGroupsSynced=:shadeGroupsSynced, verified=:verified WHERE email=:email");
    query.bindValue(":idToken", user.idToken());
    query.bindValue(":idTokenExpiration", user.idTokenExpiration());
    query.bindValue(":accessToken", user.idToken());
    query.bindValue(":accessTokenExpiration", user.accessTokenExpiration());
    query.bindValue(":refreshToken", user.idToken());
    query.bindValue(":refreshTokenExpiration", user.refreshTokenExpiration());
    query.bindValue(":locationsModified", user.locationsModified());
    query.bindValue(":controllersModified", user.controllersModified());
    query.bindValue(":shadeGroupsModified", user.shadeGroupsLastModified());
    query.bindValue(":locationsSyncCount", user.locationsSyncCount());
    query.bindValue(":controllersSyncCount", user.controllersSyncCount());
    query.bindValue(":shadeGroupsSyncCount", user.shadeGroupsSyncCount());
    query.bindValue(":locationsSynced", user.locationsSynced() ? 1 : 0);
    query.bindValue(":controllersSynced", user.controllersSynced() ? 1 : 0);
    query.bindValue(":shadeGroupsSynced", user.shadeGroupsSynced() ? 1 : 0);
    query.bindValue(":email", user.email());
    query.bindValue(":verified", user.verified() ? 1 : 0);
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
        user.setLocationsSyncCount(query.value("locationsSyncCount").toInt());
        user.setControllersSyncCount(query.value("controllersSyncCount").toInt());
        user.setShadeGroupsSyncCount(query.value("shadeGroupsSyncCount").toInt());
        user.setLocationsSynced(query.value("locationsSynced").toBool());
        user.setControllersSynced(query.value("controllersSynced").toBool());
        user.setShadeGroupsSynced(query.value("shadeGroupsSynced").toBool());
        user.setVerified(query.value("verified").toBool());
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
    query.prepare("UPDATE users SET locationsModified=:lm, controllersModified=:cm, shadeGroupsModified=:gm, locationsSyncCount=:lsc, controllersSyncCount=:csc, shadeGroupsSyncCount=:sgsc, locationsSynced=:ls, controllersSynced=:cs, shadeGroupsSynced=:sgs, verified=:verified WHERE email=:email");
    query.bindValue(":lm", user.locationsModified());
    query.bindValue(":cm", user.controllersModified());
    query.bindValue(":gm", user.shadeGroupsLastModified());
    query.bindValue(":lsc", user.locationsSyncCount());
    query.bindValue(":csc", user.controllersSyncCount());
    query.bindValue(":sgsc", user.shadeGroupsSyncCount());
    query.bindValue(":ls", user.locationsSynced());
    query.bindValue(":cs", user.controllersSynced());
    query.bindValue(":sgs", user.shadeGroupsSynced());
    query.bindValue(":email", user.email());
    query.bindValue(":verified", user.verified() ? 1 : 0);
    if(query.exec()){
        qDebug() << "UserData modified persisted";
    }
    else{
        qDebug() << "UserData modified persisting error: " << query.lastError().text() << " // SQL: " << query.lastQuery();
    }
}

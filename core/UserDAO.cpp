#include "UserDAO.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTextStream>

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
                    "refreshTokenExpiration INTEGER DEFAULT 0" \
                    ")"
                   );
    }
}

void UserDAO::createUser(User &user) const
{
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO users(email, idToken, idTokenExpiration, accessToken, accessTokenExpiration, refreshToken, refreshTokenExpiration) VALUES(:email, :idToken, :idTokenExpiration, :accessToken, :accessTokenExpiration, :refreshToken, :refreshTokenExpiration)");
    query.bindValue(":email", user.email());
    query.bindValue(":idToken", user.idToken());
    query.bindValue(":idTokenExpiration", user.idTokenExpiration());
    query.bindValue(":accessToken", user.accessToken());
    query.bindValue(":accessTokenExpiration", user.accessTokenExpiration());
    query.bindValue(":refreshToken", user.refreshToken());
    query.bindValue(":refreshTokenExpiration", user.refreshTokenExpiration());
    query.exec();
    if(query.lastError().isValid()){
        QTextStream(stdout) << "User " << user.email() << " not created: " << query.lastError().text() << endl;
    }
}

void UserDAO::updateUser(const User &user) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE users SET idToken=:idToken, idTokenExpiration=:idTokenExpiration, accessToken=:accessToken, accessTokenExpiration=:accessTokenExpiration, refreshToken=:refreshToken, refreshTokenExpiration=:refreshTokenExpiration WHERE email=:email");
    query.bindValue(":idToken", user.idToken());
    query.bindValue(":idTokenExpiration", user.idTokenExpiration());
    query.bindValue(":accessToken", user.idToken());
    query.bindValue(":accessTokenExpiration", user.accessTokenExpiration());
    query.bindValue(":refreshToken", user.idToken());
    query.bindValue(":refreshTokenExpiration", user.refreshTokenExpiration());
    query.bindValue(":email", user.email());
    query.exec();
    if(query.lastError().isValid())
        QTextStream(stdout) << "Updating user " << user.email() << "failed: " << query.lastError().text() << endl;
}

bool UserDAO::loadUser(User &user) const
{
    QSqlQuery query(mDatabase);
    query.exec("SELECT * FROM users");
    if(query.lastError().isValid()){
        QTextStream(stdout) << "Loading User error:" << query.lastError().text() << endl;
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
        return true;
    }
    else{
        QTextStream(stdout) << "No users in database." << endl;
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

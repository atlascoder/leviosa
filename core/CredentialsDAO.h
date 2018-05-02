#ifndef CREDENTIALSDAO_H
#define CREDENTIALSDAO_H

#include <QList>

class QSqlDatabase;

struct UserLogin {
    QString login;
    QString accessToken;
    QString longtermToken;
    long long longtermExpiration;
};

class CredentialsDAO
{
    QSqlDatabase & mDatabase;
public:
    CredentialsDAO(QSqlDatabase & database);
    void init() const;

    void persistLogins(const QList<UserLogin>& logins) const;
    QList<UserLogin> readLogins() const;

    void clear() const;

};

#endif // CREDENTIALSDAO_H

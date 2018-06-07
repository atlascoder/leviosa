#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <memory>
#include "UserDAO.h"
#include "CredentialsDAO.h"
#include "CognitoSyncDAO.h"
#include "LicenseDAO.h"

class QSqlDatabase;

const QString DATABASE_FILENAME = "leviosa.db";

class DatabaseManager
{
    QSqlDatabase* mDatabase;
public:
    static DatabaseManager& instance();
    ~DatabaseManager();


    const UserDAO userDAO;
    const CredentialsDAO credentialsDAO;
    const CognitoSyncDAO cognitoSyncDAO;
    const LicenseDAO licenseDAO;
    void clear() const;
protected:
    DatabaseManager(const QString &path = DATABASE_FILENAME);
    DatabaseManager& operator=(const DatabaseManager& rhs);

};

#endif // DATABASEMANAGER_H

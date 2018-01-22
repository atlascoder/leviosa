#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <memory>
#include "locationdao.h"
#include "controllerdao.h"
#include "shadesgroupdao.h"

class QSqlDatabase;

const QString DATABASE_FILENAME = "leviosa.db";

class DatabaseManager
{
    QSqlDatabase* mDatabase;
public:
    static DatabaseManager& instance();
    ~DatabaseManager();

    const LocationDao locationDao;
    const ControllerDao controllerDao;
    const ShadesGroupDao shadesGroupsDao;
protected:
    DatabaseManager(const QString &path = DATABASE_FILENAME);
    DatabaseManager& operator=(const DatabaseManager& rhs);

};

#endif // DATABASEMANAGER_H

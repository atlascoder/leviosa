#include "databasemanager.h"

#include <QSqlDatabase>

DatabaseManager::DatabaseManager(const QString& path):
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))),
    locationDao(*mDatabase),
    controllerDao(*mDatabase),
    shadesGroupsDao(*mDatabase)
{
    mDatabase->setDatabaseName(path);
    mDatabase->open();
    locationDao.init();
    controllerDao.init();
    shadesGroupsDao.init();
}

DatabaseManager::~DatabaseManager()
{
    mDatabase->close();
    delete mDatabase;
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager singleton;
    return singleton;
}

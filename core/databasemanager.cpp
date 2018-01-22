#include "databasemanager.h"

#include <QSqlDatabase>
#include <QStandardPaths>
#include <QTextStream>

DatabaseManager::DatabaseManager(const QString& path):
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))),
    locationDao(*mDatabase),
    controllerDao(*mDatabase),
    shadesGroupsDao(*mDatabase),
    userDAO(*mDatabase),
    locationsDao(*mDatabase),
    controllersDao(*mDatabase),
    shadeGroupsDao(*mDatabase)
{
#ifdef Q_OS_DARWIN
    QString dbLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QTextStream(stdout) << "DB location: " << dbLocation << "/" << path << endl;
    mDatabase->setDatabaseName(dbLocation + "/" + path);
#else
    mDatabase->setDatabaseName(path);
#endif
    mDatabase->open();
    locationDao.init();
    controllerDao.init();
    shadesGroupsDao.init();
    userDAO.init();
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

void DatabaseManager::clear() const
{
    shadesGroupsDao.clear();
    controllerDao.clear();
    locationDao.clear();
    userDAO.clear();
}

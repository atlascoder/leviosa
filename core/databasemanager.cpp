#include "databasemanager.h"

#include <QSqlDatabase>
#include <QStandardPaths>
#include <QTextStream>

DatabaseManager::DatabaseManager(const QString& path):
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))),
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
    userDAO.init();
    locationsDao.init();
    controllersDao.init();
    shadeGroupsDao.init();
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
    shadeGroupsDao.clear(false);
    controllersDao.clear(false);
    locationsDao.clear(false);
    userDAO.clear();
}

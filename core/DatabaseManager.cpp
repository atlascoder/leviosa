#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QStandardPaths>
#include <QTextStream>

DatabaseManager::DatabaseManager(const QString& path):
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))),
    userDAO(*mDatabase),
    credentialsDAO(*mDatabase),
    cognitoSyncDAO(*mDatabase)
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
    credentialsDAO.init();
    cognitoSyncDAO.init();
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
    userDAO.clear();
    credentialsDAO.clear();
    cognitoSyncDAO.clear();
}

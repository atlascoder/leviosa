#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <memory>
#include "UserDAO.h"
#include "BasicDAO.h"

class QSqlDatabase;

const QString DATABASE_FILENAME = "leviosa.db";

class DatabaseManager
{
    QSqlDatabase* mDatabase;
public:
    static DatabaseManager& instance();
    ~DatabaseManager();


    const UserDAO userDAO;
    const BasicDAO<Location> locationsDao;
    const BasicDAO<Controller> controllersDao;
    const BasicDAO<ShadeGroup> shadeGroupsDao;

    void clear() const;
protected:
    DatabaseManager(const QString &path = DATABASE_FILENAME);
    DatabaseManager& operator=(const DatabaseManager& rhs);

};

#endif // DATABASEMANAGER_H

#ifndef USERDAO_H
#define USERDAO_H

#include "User.h"
class QSqlDatabase;

class UserDAO
{

    QSqlDatabase & mDatabase;
public:
    UserDAO(QSqlDatabase & database);
    void init() const;

    void persistUser(User & user) const;
    void clear() const;
    bool loadUser(User & user) const;

    void persistUserDataModified(const User& user) const;

private:
    void createUser(User & user) const;
    void updateUser(const User & user) const;
};

#endif // USERDAO_H

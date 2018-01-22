#ifndef LOCATIONDAO_H
#define LOCATIONDAO_H

#include <QVector>
#include <memory>
#include "userlocation.h"
class QSqlDatabase;

class LocationDao
{
public:
    LocationDao(QSqlDatabase &database);
    void init() const;

    void addLocation(UserLocation &location) const;
    void updateLocation(const UserLocation &location) const;
    void removeLocation(UserLocation &location) const;
    std::unique_ptr<std::vector<std::unique_ptr<UserLocation>>> locations() const;
private:
    QSqlDatabase &mDatabase;
};

#endif // LOCATIONDAO_H

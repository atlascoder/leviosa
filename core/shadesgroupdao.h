#ifndef SHADESGROUPDAO_H
#define SHADESGROUPDAO_H

#include "shadegroup.h"
#include <memory>

class QSqlDatabase;

class ShadesGroupDao
{
    QSqlDatabase& mDatabase;
public:
    explicit ShadesGroupDao(QSqlDatabase& database);
    void init() const;

    void addShadesGroup(ShadeGroup& shadesGroup) const;
    void addShadesGroupInController(int controllerId, ShadeGroup& shadesGroup) const;

    void removeShadesGroup(int id) const;
    void updateShadesGroup(const ShadeGroup& shadesGroup) const;
    void removeShadesGroupsFromController(int controllerId) const;
    std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> shadesGroupsForController(int controllerId) const;

    void clear() const;
};

#endif // SHADESGROUPDAO_H

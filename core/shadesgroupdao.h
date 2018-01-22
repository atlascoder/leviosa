#ifndef SHADESGROUPDAO_H
#define SHADESGROUPDAO_H

#include "shadesgroup.h"
#include <memory>

class QSqlDatabase;

class ShadesGroupDao
{
    QSqlDatabase& mDatabase;
public:
    explicit ShadesGroupDao(QSqlDatabase& database);
    void init() const;

    void addShadesGroup(ShadesGroup& shadesGroup) const;
    void addShadesGroupInController(int controllerId, ShadesGroup& shadesGroup) const;

    void removeShadesGroup(int id) const;
    void updateShadesGroup(const ShadesGroup& shadesGroup) const;
    void removeShadesGroupsFromController(int controllerId) const;
    std::unique_ptr<std::vector<std::unique_ptr<ShadesGroup>>> shadesGroupsForController(int controllerId) const;
};

#endif // SHADESGROUPDAO_H

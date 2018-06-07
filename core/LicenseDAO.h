#ifndef LICENSEDAO_H
#define LICENSEDAO_H

#include <QSqlDatabase>

class LicenseDAO
{
    QSqlDatabase& mDatabase;
public:
    LicenseDAO(QSqlDatabase& database) : mDatabase(database) {}

    void init() const;
    void clear() const;

    bool agreed() const;
    void persistAgree() const;

};

#endif // LICENSEDAO_H

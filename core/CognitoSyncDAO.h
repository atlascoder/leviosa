#ifndef COGNITOSYNCDAO_H
#define COGNITOSYNCDAO_H

#include <memory>

#include "Syncable.h"

class QSqlDatabase;

using namespace std;

class CognitoSyncDAO
{
    QSqlDatabase& mDatabase;
public:
    CognitoSyncDAO(QSqlDatabase& database) : mDatabase(database) {}

    void init() const;
    void clear() const;

    long long lastSyncCount() const;

    shared_ptr<vector<shared_ptr<Syncable>>> items() const;
    shared_ptr<vector<shared_ptr<Syncable>>> modified() const;

    void persistItems(const shared_ptr<vector<shared_ptr<Syncable>>>& items, bool changedOnly = false) const;
    void persistChanged(const shared_ptr<vector<shared_ptr<Syncable>>>& items) const;
    void persistSynced(const shared_ptr<vector<shared_ptr<Syncable>>>& items) const;

private:
    shared_ptr<vector<shared_ptr<Syncable>>> select(const QString& condition = "") const;
};

#endif // COGNITOSYNCDAO_H

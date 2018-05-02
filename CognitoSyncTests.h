#ifndef COGNITOSYNCTESTS_H
#define COGNITOSYNCTESTS_H

#include <QObject>
#include <QString>

#include "core/Syncable.h"
#include <memory>

using namespace std;

class CognitoSyncTests : public QObject
{
    Q_OBJECT
    QString mDataset;
    shared_ptr<vector<shared_ptr<Syncable>>> mLocalCache;
    shared_ptr<vector<shared_ptr<Syncable>>> mExpectedCache;
public:
    CognitoSyncTests();

public slots:
    void runTests();

private:
    bool firstSyncWithV1();
};

#endif // COGNITOSYNCTESTS_H

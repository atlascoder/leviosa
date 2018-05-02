#include "CognitoSyncTests.h"

#include <QDebug>
#include <QUuid>
#include <memory>

#include "aws/CognitoSyncAPI.h"
#include "aws/AwsApi.h"
#include "core/LocalCache.h"

using namespace Aws;

CognitoSyncTests::CognitoSyncTests():
    mLocalCache(new vector<shared_ptr<Syncable>>),
    mExpectedCache(new vector<shared_ptr<Syncable>>)
{ }

bool compareCaches(const shared_ptr<vector<shared_ptr<Syncable>>>& a, const shared_ptr<vector<shared_ptr<Syncable>>>& b, bool ignoreSyncCount = true)
{
    if (a->size() != b->size()) {
        qDebug() << "Comaring caches failed: sizes are different";
        return false;
    }

    for (size_t i = 0; i < a->size(); i++) {
        if (
                a->at(i)->uuid() == b->at(i)->uuid() &&
                a->at(i)->value() == b->at(i)->value() &&
                (ignoreSyncCount || (a->at(i)->syncCount() == b->at(i)->syncCount()))
            ) continue;
        qDebug() << "Comparing caches failed : caches at(" << i << ") - do not match";
        return false;
    }
    return true;
}

bool CognitoSyncTests::firstSyncWithV1()
{
    mLocalCache->clear();
    shared_ptr<Syncable> i1(new Syncable);
    i1->setValue("Item 1");
    mLocalCache->push_back(i1);

    shared_ptr<Syncable> i2(new Syncable);
    i2->setValue("Item 2");
    mLocalCache->push_back(i2);

    mExpectedCache->clear();

    shared_ptr<Syncable> i1s(new Syncable(*i1));
    i1s->setChanged(false);
    mExpectedCache->push_back(i1s);

    shared_ptr<Syncable> i2s(new Syncable(*i2));
    i2s->setChanged(false);
    mExpectedCache->push_back(i2s);

    CognitoSyncAPI syncer;
    syncer.syncData(mLocalCache);

    if (syncer.isSuccessful()) {
        qDebug() << "Test sync successfull";
        if (!compareCaches(mLocalCache, mExpectedCache)) {
            return false;
        }
    }
    else {
        qDebug() << "Test sync failed: " << syncer.getLastMessage();
        return false;
    }
    return true;
}

void CognitoSyncTests::runTests()
{
    if (firstSyncWithV1()) {
        qDebug() << "PASS: firstSyncWithV1";
    }
    else {
        qDebug() << "FAIL: firstSyncWithV1";
        return;
    }


}

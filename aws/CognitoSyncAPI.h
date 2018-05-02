#ifndef COGNITOSYNCAPI_H
#define COGNITOSYNCAPI_H

#include <aws/cognito-sync/CognitoSyncClient.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-sync/model/UpdateRecordsRequest.h>
#include <aws/cognito-sync/model/Record.h>

#include <memory>

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QMutex>

#include "core/LocalCache.h"
#include "ClientConfig.h"

#include "core/Location.h"
#include "core/Controller.h"
#include "core/ShadeGroup.h"

using namespace std;

class CognitoSyncAPI
{
    bool mIsSuccessful;
    bool mIsUpdated;
    QString mLastMessage;
public:
    void cancelRequests();

    void syncData(const shared_ptr<vector<shared_ptr<Syncable>>>& cacheList);

    bool isSuccessful() const { return mIsSuccessful; }
    bool isUpdated() const { return mIsUpdated; }
    QString getLastMessage() const { return mLastMessage; }
};

#endif // COGNITOSYNCAPI_H

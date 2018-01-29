#ifndef COGNITOSYNCAPI_H
#define COGNITOSYNCAPI_H

#include <aws/cognito-sync/CognitoSyncClient.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-sync/model/UpdateRecordsRequest.h>
#include <aws/cognito-sync/model/Record.h>
#include <QObject>
#include <QString>
#include <QJsonObject>

#include "core/SyncableRecord.h"
#include "ClientConfig.h"

#include "core/location.h"
#include "core/controller.h"
#include "core/shadegroup.h"

class CognitoSyncAPI
{
    bool mCancelled;
    Aws::CognitoSync::CognitoSyncClient* mClient;
public:
    CognitoSyncAPI();
    ~CognitoSyncAPI();
    void resetWithCredentials(const Aws::Auth::AWSCredentials & credentials);
    void cancelRequests();
    void sync(const Aws::Auth::AWSCredentials & credentials, const QString & datasetName, SyncableRecord<Location> & locations, SyncableRecord<Controller> & controllers, SyncableRecord<ShadeGroup> & shadeGroups);
private:
    template<class T>
    void syncRecord(SyncableRecord<T> & syncable, const Aws::CognitoSync::Model::Record & record, Aws::CognitoSync::Model::UpdateRecordsRequest & update) const;
};

#endif // COGNITOSYNCAPI_H

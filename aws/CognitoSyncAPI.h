#ifndef COGNITOSYNCAPI_H
#define COGNITOSYNCAPI_H

#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-sync/model/UpdateRecordsRequest.h>
#include <aws/cognito-sync/model/Record.h>
#include <QObject>
#include <QString>
#include <QJsonObject>
#include "core/Syncable.h"

class CognitoSyncAPI
{

public:
    CognitoSyncAPI(){}
    void sync(const Aws::Auth::AWSCredentials & credentials, const QString & datasetName, std::vector<std::unique_ptr<Syncable>> * localDataset, std::vector<std::unique_ptr<Syncable>> * out, std::vector<int> * deleted);
    void sync(const Aws::Auth::AWSCredentials & credentials, const QString & datasetName, Syncable & locations, Syncable & controllers, Syncable & shadeGroups);
private:
    void syncRecord(Syncable & syncable, const Aws::CognitoSync::Model::Record & record, Aws::CognitoSync::Model::UpdateRecordsRequest & update) const;
};

#endif // COGNITOSYNCAPI_H

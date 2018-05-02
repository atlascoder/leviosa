#ifndef COGNITOSYNCCOMMAND_H
#define COGNITOSYNCCOMMAND_H

#include <QObject>
#include <QString>
#include <QMutex>

#include <aws/identity-management/auth/PersistentCognitoIdentityProvider.h>
#include <aws/cognito-sync/CognitoSyncClient.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>

class CognitoSyncCommand
{
    bool mIsSuccessful;
public:
    void sendCommand(const QString& controllerMac, const QString& command, int channel);
    bool isSuccessful() const { return mIsSuccessful; }
};

#endif // COGNITOSYNCCOMMAND_H

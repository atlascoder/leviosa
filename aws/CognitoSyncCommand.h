#ifndef COGNITOSYNCCOMMAND_H
#define COGNITOSYNCCOMMAND_H

#include <QObject>
#include <QString>

#include <aws/cognito-sync/CognitoSyncClient.h>

class CognitoSyncCommand : public QObject
{
    Q_OBJECT

    bool mCancelled;
    Aws::CognitoSync::CognitoSyncClient* mClient;
public:
    CognitoSyncCommand(QObject *parent=0);
    ~CognitoSyncCommand();

    void cancelRequests();

signals:
    void commandSent();
    void commandFailed();

public slots:
    void sendCommand(const QString& controllerMac, const QString& command, int channel);

private:
    void resetWithCredentials(const Aws::Auth::AWSCredentials & credentials);

};

#endif // COGNITOSYNCCOMMAND_H

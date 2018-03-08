#ifndef CONTROLLERTHING_H
#define CONTROLLERTHING_H

#include <QObject>
#include <QByteArray>
#include <aws/iot/IoTClient.h>

class ControllerThing
{
    bool mCancelled;

    QByteArray mPubKey;
    QByteArray mPriKey;
    QByteArray mCert;
    bool mIsSuccessful;
    QString userId;

    Aws::IoT::IoTClient* mClient;
public:
    ControllerThing();
    ~ControllerThing();

    QByteArray& pubKey() { return mPubKey; }
    QByteArray& priKey() { return mPriKey; }
    QByteArray& cert() { return mCert; }
    bool isSuccessful() const { return mIsSuccessful; }

    void cancelRequests();
    void setupController(const QString& mac);
    void resetWithCredentials(const Aws::Auth::AWSCredentials &credentials);
};

#endif // CONTROLLERTHING_H

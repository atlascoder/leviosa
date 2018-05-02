#ifndef CONTROLLERTHING_H
#define CONTROLLERTHING_H

#include <QObject>
#include <QByteArray>
#include <aws/iot/IoTClient.h>
#include <QMutex>

class ControllerThing
{
    bool mCancelled;

    QByteArray mPubKey;
    QByteArray mPriKey;
    QByteArray mCert;
    bool mIsSuccessful;
    QString userId;

    QMutex mPrepareMutex;
    QMutex mDestroyMutex;

    std::shared_ptr<Aws::IoT::IoTClient> mClient;


public:
    ControllerThing();
    ~ControllerThing();

    QByteArray& pubKey() { return mPubKey; }
    QByteArray& priKey() { return mPriKey; }
    QByteArray& cert() { return mCert; }
    bool isSuccessful() const { return mIsSuccessful; }

    void cancelRequests();
    void setupController(const QString& mac);
private:
    void buildClient();
    void destroyClient();
};

#endif // CONTROLLERTHING_H

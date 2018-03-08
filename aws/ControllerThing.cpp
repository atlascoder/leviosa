#include "ControllerThing.h"

#include "ClientConfig.h"

#include <QUuid>
#include <QDebug>

#include <aws/core/utils/Outcome.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/iot/model/CreateThingRequest.h>
#include <aws/iot/model/CreateThingResult.h>
#include <aws/iot/model/DescribeThingRequest.h>
#include <aws/iot/model/DescribeThingResult.h>
#include <aws/iot/model/UpdateThingRequest.h>
#include <aws/iot/model/UpdateThingResult.h>
#include <aws/iot/model/AttributePayload.h>
#include <aws/iot/model/CreateKeysAndCertificateRequest.h>
#include <aws/iot/model/CreateKeysAndCertificateResult.h>
#include <aws/iot/model/AttachThingPrincipalRequest.h>
#include <aws/iot/model/AttachThingPrincipalResult.h>

using namespace Aws;
using namespace IoT;

ControllerThing::ControllerThing(): mCancelled(false)
{
    mClient = New<IoTClient>(nullptr, ClientConfig::instance());
}

ControllerThing::~ControllerThing()
{
    cancelRequests();
    Delete<IoTClient>(mClient);
}

void ControllerThing::cancelRequests()
{
    mCancelled = true;
    mClient->DisableRequestProcessing();
}

void ControllerThing::resetWithCredentials(const Auth::AWSCredentials &credentials)
{
    cancelRequests();
    Delete<IoTClient>(mClient);
    mClient = New<IoTClient>(nullptr, credentials, ClientConfig::instance());
    mCancelled = false;
}

void ControllerThing::setupController(const QString &mac)
{
    mIsSuccessful = false;
    QString device_uuid = QUuid::createUuid().toString();
    device_uuid.remove("{");
    device_uuid.remove("}");

    Model::CreateThingRequest create_req;
    create_req.SetThingName(String(device_uuid.toStdString().c_str()));
    Model::AttributePayload attrs;
    attrs.AddAttributes("mac", String(mac.toStdString().c_str()));
    create_req.SetAttributePayload(attrs);

    if(mCancelled) return;
    auto create_resp = mClient->CreateThing(create_req);
    if(create_resp.IsSuccess()){
        // device created, creating certificates now
        Model::CreateKeysAndCertificateRequest create_kc_req;
        create_kc_req.SetSetAsActive(true);
        if(mCancelled) return;
        auto create_kc_resp = mClient->CreateKeysAndCertificate(create_kc_req);
        if(create_kc_resp.IsSuccess()){
            mPubKey = QByteArray::fromStdString(create_kc_resp.GetResult().GetKeyPair().GetPublicKey().c_str());
            mPriKey = QByteArray::fromStdString(create_kc_resp.GetResult().GetKeyPair().GetPrivateKey().c_str());
            mCert = QByteArray::fromStdString(create_kc_resp.GetResult().GetCertificatePem().c_str());
            Model::AttachThingPrincipalRequest attach_req;
            attach_req.SetThingName(String(device_uuid.toStdString().c_str()));
            attach_req.SetPrincipal(create_kc_resp.GetResult().GetCertificateArn());
            if(mCancelled) return;
            auto attach_resp = mClient->AttachThingPrincipal(attach_req);
            if(attach_resp.IsSuccess()){
                mIsSuccessful = true;
            }
            else
                qDebug() << "An Error while attaching cert: " << attach_resp.GetError().GetMessage().c_str();
        }
        else
            qDebug() << "An Error while creating keys: " << create_resp.GetError().GetMessage().c_str();
    }
    else
        qDebug() << "An Error while creating thing: " << create_resp.GetError().GetMessage().c_str();
}

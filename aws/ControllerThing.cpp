#include "ControllerThing.h"

#include "ClientConfig.h"

#include <QUuid>
#include <QDebug>
#include <QMutexLocker>

#include <aws/core/utils/Outcome.h>
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
#include <aws/iot/model/AttachPolicyRequest.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>

#include "aws/AwsApi.h"

#include "AlertBox.h"

using namespace Aws;

ControllerThing::ControllerThing(): mCancelled(false), mClient(nullptr)
{
}

ControllerThing::~ControllerThing()
{
    cancelRequests();
    destroyClient();
}

void ControllerThing::buildClient()
{
    QMutexLocker lock(&mPrepareMutex);
    if (mClient || mCancelled) return;
    mClient = AwsApi::instance().getClient<IoT::IoTClient>();
}

void ControllerThing::destroyClient()
{
    QMutexLocker lock(&mDestroyMutex);
    if (mClient) {
        mClient->DisableRequestProcessing();
        AwsApi::instance().recycleClient(mClient);
        mClient = nullptr;
    }
}

void ControllerThing::cancelRequests()
{
    mCancelled = true;
    if (mClient) mClient->DisableRequestProcessing();
}

void ControllerThing::setupController(const QString &mac)
{
    mIsSuccessful = false;
    QString device_uuid = QUuid::createUuid().toString();
    device_uuid.remove("{");
    device_uuid.remove("}");

    IoT::Model::CreateThingRequest create_req;
    create_req.SetThingName(String(device_uuid.toStdString().c_str()));
    IoT::Model::AttributePayload attrs;
    attrs.AddAttributes("mac", String(mac.toStdString().c_str()));
    attrs.AddAttributes("identity", AwsApi::instance().getIdentityId());
    create_req.SetAttributePayload(attrs);

    buildClient();
    if(mCancelled) return;
    auto create_resp = mClient->CreateThing(create_req);
    if(create_resp.IsSuccess()){
        // device created, creating certificates now
        IoT::Model::CreateKeysAndCertificateRequest create_kc_req;
        create_kc_req.SetSetAsActive(true);
        if(mCancelled) return;
        auto create_kc_resp = mClient->CreateKeysAndCertificate(create_kc_req);
        if(create_kc_resp.IsSuccess()){
            mPubKey = QByteArray::fromStdString(create_kc_resp.GetResult().GetKeyPair().GetPublicKey().c_str());
            mPriKey = QByteArray::fromStdString(create_kc_resp.GetResult().GetKeyPair().GetPrivateKey().c_str());
            mCert = QByteArray::fromStdString(create_kc_resp.GetResult().GetCertificatePem().c_str());
            IoT::Model::AttachThingPrincipalRequest attach_req;
            attach_req.SetThingName(String(device_uuid.toStdString().c_str()));
            attach_req.SetPrincipal(create_kc_resp.GetResult().GetCertificateArn());
            if(mCancelled) return;
            auto attach_resp = mClient->AttachThingPrincipal(attach_req);
            if(attach_resp.IsSuccess()){
                IoT::Model::AttachPolicyRequest attach_policy;
                attach_policy.SetPolicyName("ConnectAndSubscribe");
                attach_policy.SetTarget(create_kc_resp.GetResult().GetCertificateArn());
                if(mCancelled) return;
                auto policy_resp = mClient->AttachPolicy(attach_policy);
                if(policy_resp.IsSuccess()){
                    mIsSuccessful = true;
                }
                else {
                    AlertBox::instance().showMessage(
                                "Controller Setup Error",
                                create_resp.GetError().GetMessage().c_str(),
                                "Check Internet connection or restart controller setup, preferrbly, relogin firstly"
                            );
                    qDebug() << "An Error while attaching policy: " << policy_resp.GetError().GetMessage().c_str();
                }
            }
            else {
                AlertBox::instance().showMessage(
                            "Controller Setup Error",
                            create_resp.GetError().GetMessage().c_str(),
                            "Check Internet connection or restart controller setup, preferrbly, relogin firstly"
                        );
                qDebug() << "An Error while attaching cert: " << attach_resp.GetError().GetMessage().c_str();
            }
        }
        else {
            AlertBox::instance().showMessage(
                        "Controller Setup Error",
                        create_resp.GetError().GetMessage().c_str(),
                        "Check Internet connection or restart controller setup, preferrbly, relogin firstly"
                    );
            qDebug() << "An Error while creating keys: " << create_resp.GetError().GetMessage().c_str();
        }
    }
    else {
        AlertBox::instance().showMessage(
                    "Controller Setup Error",
                    create_resp.GetError().GetMessage().c_str(),
                    "Check Internet connection or restart controller setup, preferrbly, relogin firstly"
                );
        qDebug() << "An Error while creating thing: " << create_resp.GetError().GetMessage().c_str();
    }
    destroyClient();
}

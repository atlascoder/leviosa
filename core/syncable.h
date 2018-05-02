#ifndef SYNCABLE_H
#define SYNCABLE_H

#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>

class QJsonObject;

class Syncable
{
    QString mUuid;
    bool mIsChanged;
    bool mIsDeleted;
    int mSyncCount;
    QString mValue;
public:
    Syncable(const QString& uuid) : mUuid(uuid), mIsChanged(false), mIsDeleted(false), mSyncCount(0), mValue("") {}
    Syncable() : mUuid(QUuid::createUuid().toString()), mIsChanged(true), mIsDeleted(false), mSyncCount(0), mValue("") {}
    Syncable(const Syncable& syncable) = default;

    QString uuid() const { return mUuid; }
    void setUuid(const QString& uuid) { mUuid = uuid; }

    bool isChanged() const { return mIsChanged; }
    void setChanged(bool isSynced) { mIsChanged = isSynced; }

    int syncCount() const { return mSyncCount; }
    void setSyncCount(int syncCount) { mSyncCount = syncCount; }

    bool isDeleted() const { return mIsDeleted; }
    void setDeleted(bool isDeleted) { mIsDeleted = isDeleted; }

    QString value() const { return mValue; }
    void setValue(const QString& value) { mValue = value; }

    virtual void withJson(const QJsonObject& obj) { Q_UNUSED(obj) }
    virtual QJsonObject toJson() const { return QJsonObject(); }
    virtual QString toJsonString() const { return QJsonDocument(toJson()).toJson(QJsonDocument::JsonFormat::Compact); }
};

#endif // SYNCABLE_H

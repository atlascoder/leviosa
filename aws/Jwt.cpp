#include "Jwt.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QStringList>
#include <QByteArray>

Jwt::Jwt(const QString & string) : mRaw(string)
{
    mValid = parse();
}

void Jwt::setRaw(const QString &raw)
{
    mRaw = raw;
    mValid = parse();
}

bool Jwt::parse()
{
    QStringList parts = mRaw.split(".");

    if(parts.length() < 3)  return false;

    QJsonParseError jsonError;
    QJsonDocument json;
    QByteArray part;

    // parsing header
    part = QByteArray::fromStdString(parts.at(0).toStdString());
    json = QJsonDocument::fromJson(QByteArray::fromBase64(part), &jsonError);

    if(jsonError.error != QJsonParseError::NoError || !json.isObject() || !parseHeader(json.object())) return false;

    // parsing payload
    part = QByteArray::fromStdString(parts.at(1).toStdString());
    json = QJsonDocument::fromJson(QByteArray::fromBase64(part), &jsonError);

    if(jsonError.error != QJsonParseError::NoError || !json.isObject() || !parsePayload(json.object())) return false;

    // check signature
    part = QByteArray::fromStdString(parts.at(2).toStdString());
    mAuthentic = checkSignature(part);

    return true;
}

bool Jwt::parseHeader(const QJsonObject &header){
    if(!header.contains("kid")) return false;

    mKeyId = header["kid"].toString();

    if(!header.contains("alg")) return false;

    mAlgorithm = header["alg"].toString();

    return true;
}

bool Jwt::parsePayload(const QJsonObject &payload)
{
    if(!payload.contains("sub")) return false;

    mSub = payload["sub"].toString();

    if(!payload.contains("iss")) return false;

    mIssuer = payload["iss"].toString();

    if(!payload.contains("iat") || !payload["iat"].isDouble()) return false;

    mIssued = payload["iat"].toInt();

    if(!payload.contains("exp") || !payload["exp"].isDouble()) return false;

    mExpiration = payload["exp"].toInt();

    return true;
}

bool Jwt::checkSignature(const QByteArray &signature)
{
    // TODO
    Q_UNUSED(signature);
    return true;
}

QString Jwt::raw() const
{
    return mRaw;
}

QString Jwt::keyId() const
{
    return mKeyId;
}

QString Jwt::algorithm() const
{
    return mAlgorithm;
}

QString Jwt::sub() const
{
    return mSub;
}

QString Jwt::issuer() const
{
    return mIssuer;
}

int Jwt::issued() const
{
    return mIssued;
}

int Jwt::expiration() const
{
    return mExpiration;
}

bool Jwt::valid() const
{
    return mValid;
}

bool Jwt::authentic() const
{
    return mAuthentic;
}

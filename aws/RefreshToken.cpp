#include "RefreshToken.h"

bool RefreshToken::parseHeader(const QJsonObject &header)
{
    if(!header.contains("cty")) return false;

    mCty = header["cty"].toString();

    if(!header.contains("enc")) return false;

    mEnc = header["enc"].toString();

    if(!header.contains("alg")) return false;

    mAlgorithm = header["alg"].toString();

    return true;
}

bool RefreshToken::parsePayload(const QJsonObject &payload)
{
    // TODO : decryption required
    return true;
}

QString RefreshToken::cty() const
{
    return mCty;
}

QString RefreshToken::enc() const
{
    return mEnc;
}

#ifndef REFRESHTOKEN_H
#define REFRESHTOKEN_H

#include "Jwt.h"

class RefreshToken : public Jwt
{

    QString mCty;
    QString mEnc;
    bool parseHeader(const QJsonObject &header) override;
    bool parsePayload(const QJsonObject &payload) override;
public:
    RefreshToken() : Jwt() {}
    RefreshToken(const QString & string) : Jwt(string) {}

    QString cty() const;
    QString enc() const;
    QString alg() const;
};

#endif // REFRESHTOKEN_H

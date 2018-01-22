#ifndef IDTOKEN_H
#define IDTOKEN_H

#include "Jwt.h"

class IdToken : public Jwt
{

    QString mUsername;
    QString mAud;
    int mAuthTime;
    QString mEmail;
    bool mEmailVerified;

    bool parsePayload(const QJsonObject &payload) override;
public:
    IdToken() : Jwt() {}
    IdToken(const QString & string): Jwt(string) {}

    QString username() const;
    QString aud() const;
    int authTime() const;
    QString email() const;
    bool emailVerified() const;
};

#endif // IDTOKEN_H

#ifndef JWT_H
#define JWT_H

#include <QString>
#include <QJsonObject>

class Jwt
{
    bool mValid;
    bool mAuthentic;
public:
    Jwt(): mValid(false), mRaw("{}") {}
    Jwt(const QString & string);

    QString raw() const;
    void setRaw(const QString & raw);

    QString keyId() const;
    QString algorithm() const;

    QString sub() const;
    QString issuer() const;
    int issued() const;
    int expiration() const;

    bool valid() const;
    bool authentic() const;

protected:
    QString mRaw;
    QString mKeyId;
    QString mAlgorithm;

    QString mSub;
    QString mIssuer;
    int mIssued;
    int mExpiration;

    bool parse();
    virtual bool parseHeader(const QJsonObject & header);
    virtual bool parsePayload(const QJsonObject & payload);
    virtual bool checkSignature(const QByteArray & signature);
private:

};

#endif // JWT_H

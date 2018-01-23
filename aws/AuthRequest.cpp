#include "AuthRequest.h"

#include <QTextStream>
#include <QDateTime>
#include <QTimeZone>
#include <QtGlobal>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDebug>

#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/cognito-identity/CognitoIdentityRequest.h>
#include <aws/cognito-identity/model/CognitoIdentityProvider.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/cognito-idp/CognitoIdentityProviderClient.h>
#include <aws/cognito-idp/model/DescribeUserPoolClientRequest.h>
#include <aws/cognito-idp/model/SignUpRequest.h>
#include <aws/cognito-idp/model/SignUpResult.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/client/AWSError.h>
#include <aws/cognito-idp/model/InitiateAuthRequest.h>
#include <aws/cognito-idp/model/InitiateAuthResult.h>
#include <aws/cognito-idp/model/RespondToAuthChallengeRequest.h>
#include <aws/cognito-idp/model/RespondToAuthChallengeResult.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/memory/stl/AWSMap.h>

#include <aws/cognito-idp/model/ChangePasswordRequest.h>
#include <aws/cognito-idp/model/ChangePasswordResult.h>

#include <aws/cognito-idp/model/ForgotPasswordRequest.h>
#include <aws/cognito-idp/model/ForgotPasswordResult.h>

#include <boost/multiprecision/random.hpp>
#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>

#include "ClientConfig.h"

#define EPHEMERAL_KEY_LEN 1024
#define DERIVED_KEY_SIZE 16
#define DERIVED_KEY_INFO "Caldera Derived Key"

using namespace boost::multiprecision;
using namespace boost::random;

// random generator
typedef boost::random::independent_bits_engine<mt19937, EPHEMERAL_KEY_LEN, cpp_int> ephem_gen_t;

// improvement of boost powm https://stackoverflow.com/questions/48233368/boostmultiprecisionpowm-differs-from-bigintegermodpow-in-java
template <class T> T my_powm(const T& a, const T& p, const T& c) {
    T r = powm(a, p, c);
    return r<0? r + c : r;
}


AuthRequest::AuthRequest()
{
    init();
}

AuthRequest::AuthRequest(const QString& email) : mEmail(email)
{
    init();
}

AuthRequest::AuthRequest(const QString& refreshToken, int exp) : mRefreshToken(refreshToken), mRefreshTokenExpiration(exp)
{
    init();
}

AuthRequest::AuthRequest(const QString &email, const QString &password) : mEmail(email), mPassword(password)
{
    init();
}

void AuthRequest::init(){
    mN = static_cast<cpp_int>("0x" \
            "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1" \
            "29024E088A67CC74020BBEA63B139B22514A08798E3404DD" \
            "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245" \
            "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED" \
            "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D" \
            "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F" \
            "83655D23DCA3AD961C62F356208552BB9ED529077096966D" \
            "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B" \
            "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9" \
            "DE2BCBF6955817183995497CEA956AE515D2261898FA0510" \
            "15728E5A8AAAC42DAD33170D04507A33A85521ABDF1CBA64" \
            "ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7" \
            "ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6B" \
            "F12FFA06D98A0864D87602733EC86A64521F2B18177B200C" \
            "BBE117577A615D6C770988C0BAD946E208E24FA074E5AB31" \
            "43DB5BFCE0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF"
        );
    mg = 2;
    QByteArray bytes;
    // mimic positive Java BigInt
    bytes.push_back('\0');
    export_bits(mN, std::back_inserter(bytes), 8);
    export_bits(mg, std::back_inserter(bytes), 8);
    QByteArray _mk(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256));
    import_bits(mk, _mk.begin(), _mk.end(), 8);
    ephem_gen_t ephem_gen;
    do {
        cpp_int ephem = ephem_gen();
        ma = ephem % mN;
        mA = powm(mg, ma, mN);
    } while(mA % mN == 0);
    mSuccessful = false;
}

QByteArray AuthRequest::authenticateUser(const QString &userId, const QString & password, const QString &saltString, const QString &srp_b, const QString &secret_block, const QString &formattedTimestamp)
{
    QByteArray authSecretBlock = QByteArray::fromBase64(secret_block.toLatin1());
    cpp_int _B = static_cast<cpp_int>(QString("0x").append(srp_b).toLatin1());
    if(_B % mN == 0){
        QTextStream(stdout) << "E authUser: B cannot be zero" << endl;
        return QByteArray();
    }
    QByteArray A;
    export_bits(mA, std::back_inserter(A), 8);
    QByteArray B;
    export_bits(_B, std::back_inserter(B), 8);
    QByteArray key = getPasswordAuthenticationKey(userId, password, A, B, saltString);

    QMessageAuthenticationCode mac(QCryptographicHash::Sha256);
    QByteArray content;
    QString poolName = QString(ClientConfig::instance().userPoolId.c_str()).split("_").at(1);
    content.insert(content.length(), poolName.toUtf8());
    content.insert(content.length(), userId.toUtf8());
    content.insert(content.length(), authSecretBlock);
    content.insert(content.length(), formattedTimestamp);
    return mac.hash(content, key, QCryptographicHash::Sha256);
}

QByteArray AuthRequest::getPasswordAuthenticationKey(const QString &userId, const QString &password, const QByteArray &A, const QByteArray &B, const QString &salt)
{

    QByteArray arr;
    // mimic java signed BigInteger
    if((A.at(0) & 0x80) != 0) arr.push_back('\0');
    arr.insert(arr.length(), A);
    // mimic java signed BigIntinteger
    if((B.at(0) & 0x80) != 0) arr.push_back('\0');
    arr.insert(arr.length(), B);

    QByteArray digest = QCryptographicHash::hash(arr, QCryptographicHash::Sha256);

    cpp_int u;
    import_bits(u, digest.begin(), digest.end(), 8);

    if(u == 0){
        qDebug() << "E getPasswordAuthKey: hash of A and B cannot be Zero";
    }

    QByteArray userIdContent;
    userIdContent.insert(0, QString(AWS_POOL_NAME).append(userId).append(":").append(password));

    QByteArray userIdHash = QCryptographicHash::hash(userIdContent, QCryptographicHash::Sha256);

    QByteArray _salt = QByteArray::fromHex(salt.toLatin1());

    QByteArray xArr;
    xArr.insert(xArr.length(), _salt);
    xArr.insert(xArr.length(), userIdHash);
    if((xArr.at(0) & 0x80) != 0) xArr.push_front('\0');

    QByteArray xDigest = QCryptographicHash::hash(xArr, QCryptographicHash::Sha256);

    cpp_int x;
    import_bits(x, xDigest.begin(), xDigest.end(), 8);

    cpp_int biB;
    import_bits(biB, B.begin(), B.end(), 8);

    cpp_int x1 = boost::multiprecision::powm(mg, x, mN);
    cpp_int x2 = biB - mk*x1;
    cpp_int x3 = ma + u*x;
    cpp_int x4 = my_powm(x2, x3, mN);

    QByteArray _x4;
    export_bits(x4, std::back_inserter(_x4), 8);

    cpp_int S = x4 % mN;

    CryptoPP::HKDF<CryptoPP::SHA256> hkdf;

    unsigned char derived[DERIVED_KEY_SIZE];
    char infostr[20] = DERIVED_KEY_INFO;

    QByteArray ikm;
    export_bits(S, std::back_inserter(ikm), 8);
    if((ikm.at(0) & 0x80) != 0) ikm.push_front('\0');
    const unsigned char *pikm = (const unsigned char*)ikm.data();

    QByteArray slt;
    export_bits(u, std::back_inserter(slt), 8);
    if((slt.at(0) & 0x80) != 0) slt.push_front('\0');
    const unsigned char *pslt = (const unsigned char*)slt.data();

    hkdf.DeriveKey(derived, sizeof(derived), pikm, ikm.length(), pslt, slt.length(), (const unsigned char*)&infostr, strlen(infostr));
    QByteArray derivedKey = QByteArray((const char*)derived, DERIVED_KEY_SIZE);

    return derivedKey;
}

void AuthRequest::signIn()
{
    Aws::CognitoIdentityProvider::CognitoIdentityProviderClient mClient(ClientConfig::instance());

    // https://stackoverflow.com/questions/42389823/aws-sign-in-invalidparameterexception-missing-required-parameter-srp-a
    Aws::Map<Aws::String, Aws::String> authParameters;
    authParameters.insert(std::pair<Aws::String, Aws::String>("USERNAME", mEmail.toStdString()));
    QByteArray srp_a;
    export_bits(mA, std::back_inserter(srp_a), 8);
    if((srp_a.at(0) & 0x80) != 0) srp_a.push_front('\0');
    authParameters.insert(std::pair<Aws::String, Aws::String>("SRP_A", srp_a.toHex().data()));

    Aws::CognitoIdentityProvider::Model::InitiateAuthRequest initiateAuthRequest;
    initiateAuthRequest.SetAuthFlow(Aws::CognitoIdentityProvider::Model::AuthFlowType::USER_SRP_AUTH);
    initiateAuthRequest.SetClientId(AWS_CLIENT_ID);
    initiateAuthRequest.SetAuthParameters(authParameters);

    Aws::CognitoIdentityProvider::Model::InitiateAuthOutcome initiateAuthOutcome = mClient.InitiateAuth(initiateAuthRequest);

    QDateTime dt;
    dt.setTimeZone(QTimeZone::utc());

    QString strTime = dt.currentDateTimeUtc().toString("ddd MMM d HH:mm:ss UTC yyyy");
    QByteArray timeStr = QByteArray(strTime.toLatin1());

    if(!initiateAuthOutcome.IsSuccess()){
        mLastMessage = QString(initiateAuthOutcome.GetError().GetMessage().c_str());
        return;
    }

    QString user_id = QString(initiateAuthOutcome.GetResult().GetChallengeParameters().at("USERNAME").c_str());
    QString salt = QString(initiateAuthOutcome.GetResult().GetChallengeParameters().at("SALT").c_str());
    QString srp_b = QString(initiateAuthOutcome.GetResult().GetChallengeParameters().at("SRP_B").c_str());
    QString secret_block = QString(initiateAuthOutcome.GetResult().GetChallengeParameters().at("SECRET_BLOCK").c_str());

    QByteArray claim = authenticateUser(user_id, mPassword, salt, srp_b, secret_block, strTime);
    QByteArray claimBase64 = claim.toBase64();

    Aws::CognitoIdentityProvider::Model::RespondToAuthChallengeRequest mResp;
    mResp.SetChallengeName(initiateAuthOutcome.GetResult().GetChallengeName());
    mResp.SetClientId(AWS_CLIENT_ID);
    mResp.AddChallengeResponses("PASSWORD_CLAIM_SECRET_BLOCK", initiateAuthOutcome.GetResult().GetChallengeParameters().at("SECRET_BLOCK"));
    mResp.AddChallengeResponses("PASSWORD_CLAIM_SIGNATURE", claimBase64);
    mResp.AddChallengeResponses("USERNAME", initiateAuthOutcome.GetResult().GetChallengeParameters().at("USERNAME"));
    mResp.AddChallengeResponses("TIMESTAMP", timeStr);

    Aws::CognitoIdentityProvider::Model::RespondToAuthChallengeOutcome authResp = mClient.RespondToAuthChallenge(mResp);

    if (authResp.IsSuccess()) {
        mIdToken =  QString(authResp.GetResult().GetAuthenticationResult().GetIdToken().c_str());
        mAccessToken = QString(authResp.GetResult().GetAuthenticationResult().GetAccessToken().c_str());
        mRefreshToken = QString(authResp.GetResult().GetAuthenticationResult().GetRefreshToken().c_str());
        mRefreshTokenExpiration = QDateTime::currentSecsSinceEpoch() + 86400*3600;
        mSuccessful = true;
    } else {
        mLastMessage = QString(authResp.GetError().GetMessage().c_str());
    }
}

void AuthRequest::signUp()
{

    Aws::CognitoIdentityProvider::CognitoIdentityProviderClient
            mClient = Aws::CognitoIdentityProvider::CognitoIdentityProviderClient(ClientConfig::instance());

    Aws::CognitoIdentityProvider::Model::DescribeUserPoolClientRequest describeUserPoolReq;
    describeUserPoolReq.WithUserPoolId(ClientConfig::instance().userPoolId).WithClientId(ClientConfig::instance().clientId);

    Aws::CognitoIdentityProvider::Model::DescribeUserPoolClientOutcome describeUPOut;
    describeUPOut = mClient.DescribeUserPoolClient(describeUserPoolReq);

    Aws::CognitoIdentityProvider::Model::SignUpRequest mReq;
    mReq.SetClientId(AWS_CLIENT_ID);
    mReq.SetUsername(mEmail.toStdString().c_str());
    mReq.SetPassword(mPassword.toStdString().c_str());
    Aws::CognitoIdentityProvider::Model::AttributeType att_email;

    att_email.WithName("email").SetValue(mEmail.toStdString().c_str());

    mReq.AddUserAttributes(att_email);

    Aws::CognitoIdentityProvider::Model::SignUpOutcome mResult = mClient.SignUp(mReq);
    if(mResult.GetResult().GetUserConfirmed()){
        mSuccessful = true;
    }
    else{
        mLastMessage = QString(mResult.GetError().GetMessage().c_str());
    }
}

void AuthRequest::signOut()
{
    mSuccessful = true;
}

void AuthRequest::changePassword(const QString& accessToken, const QString& newPassword, const QString& password)
{
    Aws::CognitoIdentityProvider::CognitoIdentityProviderClient mClient(ClientConfig::instance());

    Aws::CognitoIdentityProvider::Model::ChangePasswordRequest req;
    req.SetAccessToken(Aws::String(accessToken.toStdString()));
    req.SetProposedPassword(Aws::String(newPassword.toStdString()));
    req.SetPreviousPassword(Aws::String(password.toStdString()));

    auto resp = mClient.ChangePassword(req);

    if(resp.IsSuccess()){
        mSuccessful = true;
    }
    else{
        mLastMessage = QString(resp.GetError().GetMessage().c_str());
    }
}

void AuthRequest::restorePassword()
{
    Aws::CognitoIdentityProvider::CognitoIdentityProviderClient client(ClientConfig::instance());

    Aws::CognitoIdentityProvider::Model::ForgotPasswordRequest req;
    req.SetClientId(ClientConfig::instance().clientId);
    req.SetUsername(Aws::String(mEmail.toStdString()));

    auto resp = client.ForgotPassword(req);
    if(resp.IsSuccess())
        mSuccessful = true;
    else
        mLastMessage = QString(resp.GetError().GetMessage().c_str());
}

bool AuthRequest::refreshTokens()
{
    int now = QDateTime::currentSecsSinceEpoch();
    if(mRefreshTokenExpiration== 0){
        return false;
    }
    else if(mRefreshTokenExpiration > now - 300){
        Aws::CognitoIdentityProvider::Model::InitiateAuthRequest initAuthReq;
        initAuthReq.SetAuthFlow(Aws::CognitoIdentityProvider::Model::AuthFlowType::REFRESH_TOKEN_AUTH);
        initAuthReq.SetClientId(AWS_CLIENT_ID);
        initAuthReq.AddAuthParameters("USERNAME", Aws::String(mEmail.toStdString()));
        initAuthReq.AddAuthParameters("REFRESH_TOKEN", Aws::String(mRefreshToken.toStdString()));

        Aws::CognitoIdentityProvider::CognitoIdentityProviderClient client(ClientConfig::instance());

        auto initAuthResp = client.InitiateAuth(initAuthReq);
        if(initAuthResp.IsSuccess()){
            mIdToken = QString(initAuthResp.GetResult().GetAuthenticationResult().GetIdToken().c_str());
            mAccessToken = QString(initAuthResp.GetResult().GetAuthenticationResult().GetAccessToken().c_str());
            return true;
        }
        else {
            mLastMessage = QString(initAuthResp.GetError().GetMessage().c_str());
        }
    }
    return false;
}

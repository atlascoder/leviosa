#include "CurrentUser.h"

#include <QDateTime>
#include <QMutexLocker>
#include "aws/CredentialsRequest.h"
#include "aws/AuthRequest.h"
#include <QtConcurrentRun>


CurrentUser::CurrentUser(QObject *parent) : QObject(parent), mDb(DatabaseManager::instance()), mIsAuthenticated(false)
{
    if(mDb.userDAO.loadUser(mUser)){
        mIdToken.setRaw(mUser.idToken());
        mAccessToken.setRaw(mUser.accessToken());
        mRefreshToken.setRaw(mUser.refreshToken());
        setAuthenticated(mUser.refreshTokenExpiration() > QDateTime::currentSecsSinceEpoch() + 300);
    }
}

CurrentUser& CurrentUser::instance()
{
    static CurrentUser sInstance;
    return sInstance;
}
void CurrentUser::askCredentials()
{
    QMutexLocker locker(&mCredentialsMutex);
    if(mCredentials.GetExpiration().Millis()/1000 < QDateTime::currentSecsSinceEpoch() - 60
            && (mCredentialsResult.isCanceled() || mCredentialsResult.isFinished()))
    {
        mCredentialsResult = QtConcurrent::run(this, &CurrentUser::requestCredentials);
    }
    else{
        emit credentialsReady();
    }
}

void CurrentUser::requestCredentials()
{
    if(mIdToken.expiration() < QDateTime::currentSecsSinceEpoch() - 60){
        // need to refresh tokens firstly
        refreshTokens();
    }

    // request credentials with idToken
    CredentialsRequest credsReq(mIdToken.raw());
    credsReq.request();
    if(credsReq.isSuccessful()){
        mCredentials = credsReq.getCredentials();
        mHasCredentials = true;
        emit credentialsReady();
    }
    else{
        mHasCredentials = false;
        emit credentialsRequestFailed(credsReq.getLastMessage());
    }
}

void CurrentUser::fillCredentials(Aws::Auth::AWSCredentials &awsCredentials)
{
    awsCredentials.SetAWSAccessKeyId(mCredentials.GetAccessKeyId());
    awsCredentials.SetAWSSecretKey(mCredentials.GetSecretKey());
    awsCredentials.SetSessionToken(mCredentials.GetSessionToken());
}

void CurrentUser::signUp(const QString& email, const QString& password)
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        mAuthResult = QtConcurrent::run(this, &CurrentUser::registerUser, email, password);
    }
}

void CurrentUser::signIn(const QString& email, const QString& password)
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        mAuthResult = QtConcurrent::run(this, &CurrentUser::authenticateUser, email, password);
    }
}

void CurrentUser::signOut()
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        mAuthResult = QtConcurrent::run(this, &CurrentUser::logoutUser);
    }
    setAuthenticated(false);
    mHasCredentials = false;
}

void CurrentUser::restorePassword()
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        mAuthResult = QtConcurrent::run(this, &CurrentUser::requestPasswordRestore);
    }

}

void CurrentUser::changePassword(const QString &newPassword, const QString &oldPassword)
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        mAuthResult = QtConcurrent::run(this, &CurrentUser::requestPasswordChange, newPassword, oldPassword);
    }
}
void CurrentUser::registerUser(const QString &email, const QString &password)
{
    AuthRequest auth(email, password);
    auth.signUp();
    if(auth.isSuccessful()){
        emit signedUp();
    }
    else{
        mLastMessage = auth.getLastMessage();
        emit authError(mLastMessage);
    }
}

void CurrentUser::authenticateUser(const QString &email, const QString &password)
{
    AuthRequest auth(email, password);
    auth.signIn();
    if(auth.isSuccessful()){
        mUser.setEmail(email);
        mIdToken.setRaw(auth.getIdToken());
        mUser.setIdToken(auth.getIdToken());
        mAccessToken.setRaw(auth.getAccessToken());
        mUser.setAccessToken(auth.getAccessToken());
        mRefreshToken.setRaw(auth.getRefreshToken());
        mUser.setRefreshToken(auth.getRefreshToken());
        mUser.setRefreshTokenExpiration(auth.getRefreshTokenExpiration());
        mUser.setLocationsModified(0);
        mUser.setControllersModified(0);
        mUser.setShadeGroupsModified(0);
        mDb.userDAO.persistUser(mUser);
        setAuthenticated(true);
        emit signedIn();
    }
    else{
        mLastMessage = auth.getLastMessage();
        setAuthenticated(false);
        emit authError(mLastMessage);
    }
}

void CurrentUser::logoutUser()
{
    AuthRequest auth;
    auth.signOut();
    if(auth.isSuccessful()){
        mDb.clear();
        emit signedOut();
    }
    else {
        emit authError(mLastMessage);
    }
}

void CurrentUser::requestPasswordRestore()
{
    AuthRequest auth(mUser.email());
    auth.restorePassword();
    if(auth.isSuccessful()){
        emit restoreRequestSent();
    }
    else {
        mLastMessage = auth.getLastMessage();
        emit authError(mLastMessage);
    }
}

void CurrentUser::requestPasswordChange(const QString& newPassword, const QString& password)
{
    AuthRequest auth;
    auth.changePassword(mAccessToken.raw(), newPassword, password);
    if(auth.isSuccessful()){
        emit restoreRequestSent();
    }
    else{
        mLastMessage = auth.getLastMessage();
        emit authError(mLastMessage);
    }
}

void CurrentUser::setEmail(const QString &email)
{
    if(email == mUser.email()) return;
    mUser.setEmail(email);
    emit emailChanged();
}

void CurrentUser::setAuthenticated(bool isAuthenticated)
{
    if(isAuthenticated == mIsAuthenticated) return;
    mIsAuthenticated = isAuthenticated;
    emit isAuthenticatedChanged();
}

void CurrentUser::refreshTokens(){
    AuthRequest req(mUser.refreshToken(), mUser.refreshTokenExpiration());
    if(req.refreshTokens()){
        mIdToken.setRaw(req.getIdToken());
        mAccessToken.setRaw(req.getAccessToken());
        setAuthenticated(true);
    }
    else{
        mLastMessage = req.getLastMessage();
        setAuthenticated(false);
    }
}

void CurrentUser::persistUserDataModified()
{
    mDb.userDAO.persistUserDataModified(mUser);
}

#include "CurrentUser.h"

#include <QDateTime>
#include <QMutexLocker>
#include "aws/CredentialsRequest.h"
#include <QtConcurrentRun>
#include <QDebug>
#include <UserData.h>


CurrentUser::CurrentUser(QObject *parent) :
    QObject(parent),
    mDb(DatabaseManager::instance()),
    mIsAuthenticated(false),
    mStopRequests(true),
    mAuthRequest(new AuthRequest),
    mCredentialsRequest(new CredentialsRequest)
{
    if(mDb.userDAO.loadUser(mUser)){
        mIdToken.setRaw(mUser.idToken());
        mAccessToken.setRaw(mUser.accessToken());
        mRefreshToken.setRaw(mUser.refreshToken());
        setAuthenticated(mUser.refreshTokenExpiration() > QDateTime::currentSecsSinceEpoch() + 300);
    }
    else
        setAuthenticated(false);

}

CurrentUser::~CurrentUser()
{
    stopRequests();
}

void CurrentUser::stopRequests()
{
    qDebug() << "CurrentUser: stop requests";
    if(mStopRequests) return;
    mStopRequests = true;
    mAuthRequest->cancelRequests();
    mCredentialsRequest->cancelRequests();
}

CurrentUser& CurrentUser::instance()
{
    static CurrentUser sInstance;
    return sInstance;
}

void CurrentUser::requestCredentials()
{
    mStopRequests = false;
    if(mIdToken.expiration() < QDateTime::currentSecsSinceEpoch() - 60){
        // need to refresh tokens firstly
        refreshTokens();
    }

    // request credentials with idToken
    mCredentialsRequest->setIdToken(mIdToken.raw());
    if(mStopRequests) return;
    mCredentialsRequest->requestId();
    if(mCredentialsRequest->isSuccessful() && !mStopRequests){
        mCredentialsRequest->requestCredentials();
        if(mCredentialsRequest->isSuccessful()){
            mCredentials = mCredentialsRequest->getCredentials();
            mHasCredentials = true;
            emit credentialsReady();
        }
        else{
            mHasCredentials = false;
            emit credentialsRequestFailed(mCredentialsRequest->getLastMessage());
        }
    }
    else{
        mHasCredentials = false;
        emit credentialsRequestFailed(mCredentialsRequest->getLastMessage());
    }
    mStopRequests = true;
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
    setEmail("");
    mDb.userDAO.clear();
    setLocationsSyncCount(0);
    setControllersSyncCount(0);
    setShadeGroupsSyncCount(0);
    mDb.locationsDao.clear(false);
    mDb.controllersDao.clear(false);
    mDb.shadeGroupsDao.clear(false);
    setAuthenticated(false);
    mHasCredentials = false;
}

void CurrentUser::restorePassword(const QString& email)
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        mAuthResult = QtConcurrent::run(this, &CurrentUser::requestPasswordRestore, email);
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

    mAuthRequest->signUp(email, password);
    if(mAuthRequest->isSuccessful()){
        emit signedUp();
        UserData::instance().createDefaultLocation();
        authenticateUser(email, password);
    }
    else{
        mLastMessage = mAuthRequest->getLastMessage();
        emit authError(mLastMessage);
    }
}

void CurrentUser::authenticateUser(const QString &email, const QString &password)
{
    mAuthRequest->signIn(email, password);
    if(mAuthRequest->isSuccessful()){
        mUser.setEmail(email);
        mIdToken.setRaw(mAuthRequest->getIdToken());
        mUser.setIdToken(mAuthRequest->getIdToken());
        mAccessToken.setRaw(mAuthRequest->getAccessToken());
        mUser.setAccessToken(mAuthRequest->getAccessToken());
        mRefreshToken.setRaw(mAuthRequest->getRefreshToken());
        mUser.setRefreshToken(mAuthRequest->getRefreshToken());
        mUser.setRefreshTokenExpiration(mAuthRequest->getRefreshTokenExpiration());
        mUser.setLocationsModified(0);
        mUser.setControllersModified(0);
        mUser.setShadeGroupsModified(0);
        mDb.userDAO.persistUser(mUser);

        setAuthenticated(true);
        emit signedIn();
        emit emailChanged();
    }
    else{
        mLastMessage = mAuthRequest->getLastMessage();
        setAuthenticated(false);
        emit authError(mLastMessage);
    }
}

void CurrentUser::logoutUser()
{
    mAuthRequest->signOut();
    if(mAuthRequest->isSuccessful()){
        mDb.clear();
        emit signedOut();
    }
    else {
        mLastMessage = mAuthRequest->getLastMessage();
        emit authError(mLastMessage);
    }
}

void CurrentUser::requestPasswordRestore(const QString& email)
{
    mAuthRequest->restorePassword(email);
    if(mAuthRequest->isSuccessful()){
        emit restoreRequestSent();
    }
    else {
        mLastMessage = mAuthRequest->getLastMessage();
        emit authError(mLastMessage);
    }
}

void CurrentUser::requestPasswordChange(const QString& newPassword, const QString& password)
{
    mAuthRequest->changePassword(mAccessToken.raw(), newPassword, password);
    if(mAuthRequest->isSuccessful()){
        emit passwordChanged();
    }
    else{
        mLastMessage = mAuthRequest->getLastMessage();
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
    emit emailChanged();
}

void CurrentUser::refreshTokens(){
    if(mAuthRequest->refreshTokens(mUser.email(), mUser.refreshToken(), mUser.refreshTokenExpiration())){
        mIdToken.setRaw(mAuthRequest->getIdToken());
        mAccessToken.setRaw(mAuthRequest->getAccessToken());
        setAuthenticated(true);
    }
    else{
        mLastMessage = mAuthRequest->getLastMessage();
        setAuthenticated(false);
    }
}

void CurrentUser::persistUserDataModified()
{
    mDb.userDAO.persistUserDataModified(mUser);
}

#include "CurrentUser.h"

#include <QDateTime>
#include <QMutexLocker>
#include "aws/CredentialsRequest.h"
#include <QtConcurrentRun>
#include <QDebug>


CurrentUser::CurrentUser(QObject *parent) :
    QObject(parent),
    mDb(DatabaseManager::instance()),
    mIsAuthenticated(false),
    mStopRequests(false),
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
    if(mStopRequests) return;
    mAuthRequest->signUp(email, password);
    if(mAuthRequest->isSuccessful()){
        emit signedUp();
    }
    else{
        mLastMessage = mAuthRequest->getLastMessage();
        emit authError(mLastMessage);
    }
}

void CurrentUser::authenticateUser(const QString &email, const QString &password)
{
    if(mStopRequests) return;
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
    }
    else{
        mLastMessage = mAuthRequest->getLastMessage();
        setAuthenticated(false);
        emit authError(mLastMessage);
    }
}

void CurrentUser::logoutUser()
{
    if(mStopRequests) return;
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

void CurrentUser::requestPasswordRestore()
{
    if(mStopRequests) return;
    mAuthRequest->restorePassword(mUser.email());
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
    if(mStopRequests) return;
    mAuthRequest->changePassword(mAccessToken.raw(), newPassword, password);
    if(mAuthRequest->isSuccessful()){
        emit restoreRequestSent();
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
    if(mStopRequests) return;
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

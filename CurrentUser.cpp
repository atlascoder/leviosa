#include "CurrentUser.h"

#include <QDateTime>
#include <QMutexLocker>
#include <QtConcurrentRun>
#include <QDebug>
#include <QList>

#include <aws/identity-management/auth/PersistentCognitoIdentityProvider.h>
#include <aws/cognito-identity/model/GetIdRequest.h>
#include <aws/cognito-identity/model/GetIdResult.h>
#include <aws/core/utils/memory/stl/AWSMap.h>
#include <memory>

#include "aws/AwsApi.h"
#include "UserData.h"


CurrentUser::CurrentUser(QObject *parent) :
    QObject(parent),
    mDb(DatabaseManager::instance()),
    mAuthState(Anon),
    mStopRequests(false),
    mAuthRequest(new AuthRequest)
{
    loadUser();
}

CurrentUser::~CurrentUser()
{
    stopRequests();
}

void CurrentUser::loadUser()
{
    mUser.clear();
    if(mDb.userDAO.loadUser(mUser)){
        setEmail(mUser.email());
        if(mUser.verified()){
            if(AwsApi::instance().HasLogins()){
                setAuthState(Authenticated);
            }
            else{
                setAuthState(Confirmed);
            }
        }
        else{
            setAuthState(Registered);
        }
    }
    else{
        setAuthState(Anon);
    }
}

void CurrentUser::stopRequests()
{
    qDebug() << "CurrentUser: stop requests";
    if(mStopRequests) return;
    mStopRequests = true;
    mAuthRequest->cancelRequests();
    setBusy(false);
}

CurrentUser& CurrentUser::instance()
{
    static CurrentUser sInstance;
    return sInstance;
}

void CurrentUser::signUp()
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        setLastMessage("Registering..");
        mAuthResult = QtConcurrent::run(this, &CurrentUser::registerUser);
    }
    else{
        setErrorMessage("Can't make request now, try later.");
    }
}

void CurrentUser::verify()
{
    mAuthResult.cancel();
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        setLastMessage("Sending verification..");
        mAuthResult = QtConcurrent::run(this, &CurrentUser::verifyEmail);
    }
    else{
        setErrorMessage("Can't make request now, try later.");
    }
}

void CurrentUser::retryConfirmation()
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        setLastMessage("Requesting new code..");
        mAuthResult = QtConcurrent::run(this, &CurrentUser::requestConfirmation);
    }
    else{
        setErrorMessage("Can't make request now, try later.");
    }
}

void CurrentUser::signIn()
{
    mAuthResult.cancel();
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        setLastMessage("Authentication...");
        mAuthResult = QtConcurrent::run(this, &CurrentUser::authenticateUser);
    }
    else{
        setErrorMessage("Can't make request now, try later.");
    }
}

void CurrentUser::signOut()
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        mAuthResult = QtConcurrent::run(this, &CurrentUser::logoutUser);
    }
    clearUser();
    setAuthState(Anon);
}

void CurrentUser::clearUser()
{
    mDb.clear();
    UserData::instance().clear();
    clearPasswords();
    setEmail("");
}

void CurrentUser::clearPasswords()
{
    mPassword = mPassword2 = mPassword3 = "";
}

void CurrentUser::restorePassword()
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        setLastMessage("Requesting restore password..");
        mAuthResult = QtConcurrent::run(this, &CurrentUser::requestPasswordRestore);
    }
    else{
        setErrorMessage("Can't make request now, try later.");
    }
}

void CurrentUser::changePassword()
{
    if(mAuthResult.isCanceled() || mAuthResult.isFinished()){
        setLastMessage("Changing password..");
        mAuthResult = QtConcurrent::run(this, &CurrentUser::requestPasswordChange);
    }
    else{
        setErrorMessage("Can't make request now, try later.");
    }
}
void CurrentUser::registerUser()
{
    setBusy(true);
    mAuthRequest->signUp(mEmail, mPassword);
    if(mAuthRequest->isSuccessful()){
        mUser.setEmail(mEmail);
        mUser.setVerified(mAuthRequest->userConfirmed());
        mDb.userDAO.persistUser(mUser);
        if(mUser.verified())
            emit signedUpConfirmed();
        else
            emit signedUpNotConfirmed();
    }
    else{
        setErrorMessage(mAuthRequest->getLastMessage());
        emit authError(mLastMessage);
    }
    setBusy(false);
}

void CurrentUser::verifyEmail()
{
    setBusy(true);
    mAuthRequest->verify(mUser.email(), mPassword3);
    if(mAuthRequest->isSuccessful()){
        mUser.setVerified(true);
        mDb.userDAO.persistUser(mUser);
        emit verified();
    }
    else{
        setErrorMessage(mAuthRequest->getLastMessage());
        emit authError(mLastMessage);
    }
    setBusy(false);
}

void CurrentUser::requestConfirmation()
{
    setBusy(true);
    mAuthRequest->requestConfirmation(mUser.email());
    if(mAuthRequest->isSuccessful()){
        setLastMessage("Request was sent. Check your mailbox for new confirmation code.");
        emit confirmationResent();
    }
    else{
        setErrorMessage(mAuthRequest->getLastMessage());
        emit authError(mLastMessage);
    }
    setBusy(false);
}

void CurrentUser::authenticateUser()
{
    setBusy(true);
    mAuthRequest->signIn(mEmail, mPassword);
    if(mAuthRequest->isSuccessful()){
        mUser.setEmail(mEmail);
        mUser.setLocationsModified(0);
        mUser.setControllersModified(0);
        mUser.setShadeGroupsModified(0);
        mUser.setVerified(true);
        mDb.userDAO.persistUser(mUser);

        Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens> logins;
        Aws::Auth::LoginAccessTokens tok;
        tok.accessToken = Aws::Utils::StringUtils::to_string(mAuthRequest->getIdToken().toStdString());
        tok.longTermToken = Aws::Utils::StringUtils::to_string(mAuthRequest->getRefreshToken().toStdString());
        tok.longTermTokenExpiry = mAuthRequest->getRefreshTokenExpiration();
        logins[ClientConfig::instance().loginPoint] = tok;
        AwsApi::instance().persistLogins(logins);

        mAuthRequest->updateIdentityIdAttribute(mAuthRequest->getAccessToken());

        setLastMessage("Authenticated");
        setAuthState(Authenticated);
        emit signedIn();
        emit emailChanged();
    }
    else{
        setErrorMessage(mAuthRequest->getLastMessage());
        emit authError(mLastMessage);
    }
    setBusy(false);
}

void CurrentUser::logoutUser()
{
    setBusy(true);
    mAuthRequest->signOut();
    AwsApi::instance().reset();
    if(mAuthRequest->isSuccessful()){
        setLastMessage("Signed out.");
        Aws::Auth::DefaultPersistentCognitoIdentityProvider prov(
                    ClientConfig::instance().identityPool,
                    ClientConfig::instance().accountId
                );
        prov.Logout();
        emit signedOut();
    }
    else {
        setErrorMessage(mAuthRequest->getLastMessage());
        emit authError(mLastMessage);
    }
    setBusy(false);
}

void CurrentUser::requestPasswordRestore()
{
    setBusy(true);
    mAuthRequest->restorePassword(mEmail);
    if(mAuthRequest->isSuccessful()){
        setLastMessage("Request has been sent, check your mailbox.");
        emit restoreRequestSent();
    }
    else {
        setErrorMessage(mAuthRequest->getLastMessage());
        emit authError(mLastMessage);
    }
    setBusy(false);
}

void CurrentUser::requestPasswordChange()
{
    setBusy(true);
    mAuthRequest->changePassword(mEmail, mPassword3, mPassword);
    if(mAuthRequest->isSuccessful()){
        setLastMessage("Password was changed");
        emit passwordChangeCompleted();
    }
    else{
        setErrorMessage(mAuthRequest->getLastMessage());
        emit authError(mLastMessage);
    }
    setBusy(false);
}

void CurrentUser::setEmail(const QString &email)
{
    QString _email = email.toLower();
    if(_email == mEmail) return;
    mEmail = _email;
    emit emailChanged();
}

void CurrentUser::persistUserDataModified()
{
    mDb.userDAO.persistUserDataModified(mUser);
}

void CurrentUser::setAuthState(const AuthState authState)
{
    if(authState == mAuthState) return;
    mAuthState = authState;
    emit authStateChanged();
}

void CurrentUser::checkEmail(){
    if(!QRegExp("[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,64}").exactMatch(mEmail)){
        setLastMessage("Enter valid email");
        setReady(false);
    }
    else{
        setReady(true);
    }
}

void CurrentUser::checkEmailPassword()
{
    if(mEmail == "change@password.my"){
        if(mPassword.length() < 8 || !QRegExp(".*\\d+.*").exactMatch(mPassword) || !QRegExp(".*\\D+.*").exactMatch(mPassword)){
            setLastMessage("Enter password comprising at least 8 letters and numbers");
            setReady(false);
        }
        else if(mPassword.length() != mPassword2.length()){
            setLastMessage("Enter the same password as confirmation");
            setReady(false);
        }
        else if(mPassword != mPassword2){
            setLastMessage("Password and its confirmation are not match");
            setReady(false);
        }
        else if(mPassword3.length() < 8){
            setLastMessage("Current password is too short");
            setReady(false);
        }
        else{
            setLastMessage("Ready");
            setReady(true);
        }
    }
    else{
        if(!QRegExp("[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,64}").exactMatch(mEmail)){
            setLastMessage("Enter valid email");
            setReady(false);
        }
        else if(mPassword.length() < 8 || !QRegExp(".*\\d+.*").exactMatch(mPassword) || !QRegExp(".*\\D+.*").exactMatch(mPassword)){
            setLastMessage("Enter password comprising at least 8 letters and numbers");
            setReady(false);
        }
        else if(mPassword.length() != mPassword2.length()){
            setLastMessage("Enter the same password as confirmation");
            setReady(false);
        }
        else if(mPassword != mPassword2){
            setLastMessage("Password and its confirmation do not match");
            setReady(false);
        }
        else{
            setLastMessage("Ready");
            setReady(true);
        }
    }
}

void CurrentUser::checkCodeAndPasswords()
{
    if(!QRegExp("\\d{6}").exactMatch(mPassword3)){
        setLastMessage("Check you email for a verification code of 6 numbers. Enter those numbers below.");
        setReady(false);
    }
    else if(mPassword.length() < 8 || !QRegExp(".*\\d+.*").exactMatch(mPassword) || !QRegExp(".*\\D+.*").exactMatch(mPassword)){
        setLastMessage("Enter password comprising at least 8 letters and numbers");
        setReady(false);
    }
    else if(mPassword.length() != mPassword2.length()){
        setLastMessage("Enter the same password as confirmation");
        setReady(false);
    }
    else if(mPassword != mPassword2){
        setLastMessage("Password and its confirmation do not match");
        setReady(false);
    }
    else{
        setLastMessage("Ready");
        setReady(true);
    }
}

void CurrentUser::checkVerification()
{
    if(!QRegExp("\\d{6}").exactMatch(mPassword3)){
        setLastMessage("Check you email for a verification code of 6 numbers. Enter those numbers below.");
        setReady(false);
    }
    else{
        setLastMessage("Ready");
        setReady(true);
    }
}

void CurrentUser::setReady(const bool ready)
{
    if(ready == mIsReady) return;
    mIsReady = ready;
    emit readyChanged();
}

void CurrentUser::setBusy(const bool busy)
{
    if(busy == mIsBusy) return;
    mIsBusy = busy;
    emit busyChanged();
}

void CurrentUser::setLastMessage(const QString &lastMessage)
{
    setFailed(false);
    if(lastMessage == mLastMessage) return;
    mLastMessage = lastMessage;
    emit lastMessageChanged();
}

void CurrentUser::setErrorMessage(const QString &errorMessage)
{
    setFailed(true);
    if(errorMessage == mLastMessage) return;
    mLastMessage = errorMessage;
    emit lastMessageChanged();
}

void CurrentUser::setPassword(const QString &password)
{
    if(password == mPassword) return;
    mPassword = password;
    emit passwordChanged();
}

void CurrentUser::setPassword2(const QString &password)
{
    if(password == mPassword2) return;
    mPassword2 = password;
    emit password2Changed();
}

void CurrentUser::setPassword3(const QString &password)
{
    if(password == mPassword3) return;
    mPassword3 = password;
    emit password3Changed();
}

void CurrentUser::setFailed(const bool failed)
{
    if(failed == mFailed) return;
    mFailed = failed;
    emit failedChanged();
}

void CurrentUser::clearInputs()
{
    clearPasswords();
    setEmail("");
}

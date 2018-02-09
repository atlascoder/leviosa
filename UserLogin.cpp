#include "UserLogin.h"
#include "UserData.h"

UserLogin::UserLogin(QObject *parent) : QObject(parent), mCurrentUser(CurrentUser::instance())
{
    connect(&mCurrentUser, &CurrentUser::signedUp, this, &UserLogin::signedUp);
    connect(&mCurrentUser, &CurrentUser::signedIn, this, &UserLogin::signedIn);
    connect(&mCurrentUser, &CurrentUser::signedOut, this, &UserLogin::signedOut);
    connect(&mCurrentUser, &CurrentUser::passwordChanged, this, &UserLogin::onPasswordChanged);
    connect(&mCurrentUser, &CurrentUser::restoreRequestSent, this, &UserLogin::onResetPasswordSent);
    connect(&mCurrentUser, &CurrentUser::authError, this, &UserLogin::authFailed);
    connect(&UserData::instance(), &UserData::syncStateChanged, this, &UserLogin::syncStateChanged);
    if(mCurrentUser.isAuthenticated())
        setAuthState(Authenticated);
}

void UserLogin::setEmail(const QString &email)
{
    if(email == mEmail) return;
    mEmail = email;
    checkEmailPassword();
    emit emailChanged();
}

void UserLogin::setPassword(const QString &password)
{
    if(password == mPassword) return;
    mPassword = password;
    checkEmailPassword();
    emit passwordChanged();
}

void UserLogin::setPassword2(const QString &password)
{
    if(password == mPassword2) return;
    mPassword2 = password;
    checkEmailPassword();
    emit password2Changed();
}

void UserLogin::setPassword3(const QString &password)
{
    if(password == mPassword2) return;
    mPassword3 = password;
    checkEmailPassword();
    emit password2Changed();
}

void UserLogin::setReadyToAuth(bool readyToAuth)
{
    if(readyToAuth == mReadyToAuth) return;
    mReadyToAuth = readyToAuth;
    emit readyToAuthChanged();
}

void UserLogin::checkEmailPassword()
{
    if(mEmail == "change@password.my"){
        if(mPassword.length() < 8 || !QRegExp(".*\\d+.*").exactMatch(mPassword) || !QRegExp(".*\\D+.*").exactMatch(mPassword)){
            setLastMessage("Enter password comprising at least 8 characters from letters AND numbers");
            setAuthState(BadInputs);
        }
        else if(mPassword.length() != mPassword2.length()){
            setLastMessage("Enter the same password as comformation");
            setAuthState(BadInputs);
        }
        else if(mPassword != mPassword2){
            setLastMessage("Password and its confirmation are not match");
            setAuthState(BadInputs);
        }
        else if(mPassword3.length() < 8){
            setLastMessage("Current password is too short");
            setAuthState(BadInputs);
        }
        else{
            setLastMessage("Ready for submit");
            setAuthState(Ready);
        }
    }
    else{
        if(!QRegExp("[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,64}").exactMatch(mEmail)){
            setLastMessage("Enter valid email");
            setAuthState(BadInputs);
        }
        else if(mPassword.length() < 8 || !QRegExp(".*\\d+.*").exactMatch(mPassword) || !QRegExp(".*\\D+.*").exactMatch(mPassword)){
            setLastMessage("Enter password comprising at least 8 characters from letters AND numbers");
            setAuthState(BadInputs);
        }
        else if(mPassword.length() != mPassword2.length()){
            setLastMessage("Enter the same password as comformation");
            setAuthState(BadInputs);
        }
        else if(mPassword != mPassword2){
            setLastMessage("Password and its confirmation are not match");
            setAuthState(BadInputs);
        }
        else{
            setLastMessage("Ready for submit");
            setAuthState(Ready);
        }
    }
}

void UserLogin::setLastMessage(const QString& message)
{
    if(message == mLastMessage) return;
    mLastMessage = message;
    emit lastMessageChanged();
}

void UserLogin::setAuthState(AuthState authState)
{
    if(authState == mAuthState) return;
    mAuthState = authState;
    emit authStateChanged();
}

void UserLogin::signIn()
{
    mCurrentUser.setEmail(mEmail);
    mCurrentUser.signIn(mEmail, mPassword);
    setAuthState(Authenticating);
    setLastMessage("User authentication..");
}

void UserLogin::signUp()
{
    mCurrentUser.setEmail(mEmail);
    mPassword3 = mPassword;
    mCurrentUser.signUp(mCurrentUser.email(), mPassword3);
    setAuthState(Registering);
    setLastMessage("User registration..");
}

void UserLogin::signOut()
{
    mCurrentUser.signOut();
    checkEmailPassword();
}

void UserLogin::changePassword()
{
    mCurrentUser.changePassword(mPassword, mPassword3);
    setLastMessage("Requesting for change..");
    setAuthState(Authenticating);
}

void UserLogin::resetPassword()
{
    mCurrentUser.restorePassword(mEmail);
    setLastMessage("Requesting for password reset..");
    setAuthState(Authenticating);
}

void UserLogin::signedUp()
{
    if(mPassword3.isEmpty()) return;
    setLastMessage("Authentication..");
    setAuthState(Authenticating);
}

void UserLogin::signedIn()
{
    setLastMessage("Downloading...");
    setAuthState(Downloading);
    UserData::instance().sync();
}

void UserLogin::signedOut()
{
    mPassword = "";
    mPassword2 = "";
    mPassword3 = "";
    checkEmailPassword();
}

void UserLogin::onPasswordChanged()
{
    setLastMessage("Password has been changed");
    setAuthState(Authenticated);
}

void UserLogin::onResetPasswordSent()
{
    setLastMessage("Request has been sent.\nCheck your mailbox shortly.");
    setAuthState(Authenticated);
}

void UserLogin::authFailed(const QString &message)
{
    setLastMessage(message);
    setAuthState(Failed);
}

void UserLogin::syncStateChanged()
{
    if(UserData::instance().syncState() != UserData::Syncing)
        setAuthState(Authenticated);
}

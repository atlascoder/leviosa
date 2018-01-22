#include "UserLogin.h"

UserLogin::UserLogin(QObject *parent) : QObject(parent), mCurrentUser(CurrentUser::instance())
{
    connect(&mCurrentUser, &CurrentUser::signedUp, this, &UserLogin::signedUp);
    connect(&mCurrentUser, &CurrentUser::signedIn, this, &UserLogin::signedIn);
    connect(&mCurrentUser, &CurrentUser::signedOut, this, &UserLogin::signedOut);
    connect(&mCurrentUser, &CurrentUser::authError, this, &UserLogin::authFailed);
    if(mCurrentUser.isAuthenticated())
        setAuthState(Authenticated);
}

void UserLogin::setEmail(const QString &email)
{
    if(email == mEmail) return;
    mEmail = email;
    checkEmailPassword();
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

void UserLogin::setReadyToAuth(bool readyToAuth)
{
    if(readyToAuth == mReadyToAuth) return;
    mReadyToAuth = readyToAuth;
    emit readyToAuthChanged();
}

void UserLogin::checkEmailPassword()
{
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
    mCurrentUser.signIn(mEmail, mPassword);
    setAuthState(Authenticating);
    setLastMessage("User authentication..");
}

void UserLogin::signUp()
{
    mCurrentUser.signUp(mEmail, mPassword);
    setAuthState(Registering);
    setLastMessage("User registration..");
}

void UserLogin::signOut()
{
    mCurrentUser.signOut();
    mEmail = "";
    mPassword = "";
    checkEmailPassword();
}

void UserLogin::signedUp()
{
    mCurrentUser.signIn(mEmail, mPassword);
    setLastMessage("Authentication..");
    setAuthState(Authenticating);
}

void UserLogin::signedIn()
{
    setAuthState(Authenticated);
}

void UserLogin::signedOut()
{
    mPassword = "";
    mPassword = "";
    checkEmailPassword();
}

void UserLogin::authFailed(const QString &message)
{
    setLastMessage(message);
    setAuthState(Failed);
}

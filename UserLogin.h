#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QObject>
#include "CurrentUser.h"

class UserLogin : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString password2 READ password2 WRITE setPassword2 NOTIFY password2Changed)
    Q_PROPERTY(QString password3 READ password3 WRITE setPassword3 NOTIFY password3Changed)
    Q_PROPERTY(bool readyToAuth READ readyToAuth NOTIFY readyToAuthChanged)
    Q_PROPERTY(QString lastMessage READ lastMessage WRITE setLastMessage NOTIFY lastMessageChanged)
    Q_PROPERTY(AuthState authState READ authState NOTIFY authStateChanged)

public:
    UserLogin(QObject * parent = 0);

    enum AuthState { BadInputs, Ready, Registering, Authenticating, Downloading, Authenticated, Failed, Completed };
    Q_ENUMS(AuthState)

signals:

    void emailChanged();
    void passwordChanged();
    void password2Changed();
    void password3Changed();
    void readyToAuthChanged();
    void lastMessageChanged();
    void authStateChanged();
public slots:
    void signUp();
    void signIn();
    void signOut();
    void changePassword();
    void resetPassword();
private slots:
    void signedUp();
    void signedIn();
    void signedOut();
    void onPasswordChanged();
    void onResetPasswordSent();

    void authFailed(const QString& message);
    void syncStateChanged();
private:
    CurrentUser& mCurrentUser;
    QString mEmail;
    QString mPassword;
    QString mPassword2;
    bool mReadyToAuth;
    AuthState mAuthState;
    QString mLastMessage;
    QString mPassword3;

    QString email() const { return mEmail; }
    void setEmail(const QString& email);

    QString password() const { return mPassword; }
    void setPassword(const QString& password);

    QString password2() const { return mPassword2; }
    void setPassword2(const QString& password);

    QString password3() const { return mPassword3; }
    void setPassword3(const QString& password);

    QString lastMessage() const { return mLastMessage; }
    void setLastMessage(const QString& message);

    AuthState authState() const { return mAuthState; }
    void setAuthState(AuthState authState);

    bool readyToAuth() const { return mReadyToAuth; }
    void setReadyToAuth(bool readyToAuth);

    void checkEmailPassword();


};

#endif // USERLOGIN_H

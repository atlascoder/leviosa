#ifndef CURRENTUSER_H
#define CURRENTUSER_H

#include <QObject>
#include <QString>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <QFuture>
#include <memory>

#include "core/DatabaseManager.h"
#include "core/User.h"
#include "aws/AuthRequest.h"

class CurrentUser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString password2 READ password2 WRITE setPassword2 NOTIFY password2Changed)
    Q_PROPERTY(QString password3 READ password3 WRITE setPassword3 NOTIFY password3Changed)

    Q_PROPERTY(AuthState authState READ authState WRITE setAuthState NOTIFY authStateChanged)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authStateChanged)

    Q_PROPERTY(QString lastMessage READ lastMessage NOTIFY lastMessageChanged)

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool failed READ failed NOTIFY failedChanged)
    Q_PROPERTY(bool requireConfirmation READ requireConfirmation NOTIFY authStateChanged)

public:
    static CurrentUser& instance();

    enum AuthState { Anon, Registered, Confirmed, Authenticated, Expired };
    Q_ENUMS(AuthState)

    QString email() const { return mEmail; }
    void setEmail(const QString& email);

    bool isAuthenticated() const { return mAuthState == Authenticated; }

    AuthState authState() const { return mAuthState; }
    void setAuthState(const AuthState authState);

    bool ready() const { return mIsReady; }

    bool busy() const { return mIsBusy; }

    bool failed() const { return mFailed; }

    bool requireConfirmation() const { return mAuthState == AuthState::Registered; }

    QString lastMessage() const { return mLastMessage; }

    int locationsModified() const { return mUser.locationsModified(); }
    void setLocationsModified(int modified) { mUser.setLocationsModified(modified); }

    int controllersModified() const { return mUser.controllersModified(); }
    void setControllersModified(int modified) { mUser.setControllersModified(modified); }

    int shadeGroupsModified() const { return mUser.shadeGroupsLastModified(); }
    void setShadeGroupsModified(int modified) { mUser.setShadeGroupsModified(modified); }

    int locationsSyncCount() const { return mUser.locationsSyncCount(); }
    void setLocationsSyncCount(int syncCount) { mUser.setLocationsSyncCount(syncCount); }

    int controllersSyncCount() const { return mUser.controllersSyncCount(); }
    void setControllersSyncCount(int syncCount) { mUser.setControllersSyncCount(syncCount); }

    int shadeGroupsSyncCount() const { return mUser.shadeGroupsSyncCount(); }
    void setShadeGroupsSyncCount(int syncCount) { mUser.setShadeGroupsSyncCount(syncCount); }

    int locationsSynced() const { return mUser.locationsSynced(); }
    void setLocationsSynced(bool synced) { mUser.setLocationsSynced(synced); }

    int controllersSynced() const { return mUser.controllersSynced(); }
    void setControllersSynced(bool synced) { mUser.setControllersSynced(synced); }

    int shadeGroupsSynced() const { return mUser.shadeGroupsSynced(); }
    void setShadeGroupsSynced(bool synced) { mUser.setShadeGroupsSynced(synced); }

    void persistUserDataModified();

    void stopRequests();

public slots:
    void signUp();
    void verify();
    void retryConfirmation();
    void signIn();
    void signOut();
    void restorePassword();
    void changePassword();

    void clearInputs();
    void checkEmail();
    void checkCodeAndPasswords();
    void checkEmailPassword();
    void checkVerification();

signals:
    void authRequired();

    void signedUpConfirmed();
    void signedUpNotConfirmed();
    void verified();
    void confirmationResent();
    void signedIn();
    void signedOut();
    void passwordChangeCompleted();

    void authError(const QString& message);
    void restoreRequestSent();

    void emailChanged();

    void passwordChanged();
    void password2Changed();
    void password3Changed();

    void isAuthenticatedChanged();
    void verifiedChanged();

    void authStateChanged();
    void readyChanged();
    void busyChanged();
    void requireConfirmationChanged();
    void failedChanged();
    void lastMessageChanged();
private:
    User mUser;
    DatabaseManager& mDb;

    QFuture<void> mAuthResult;

    AuthState mAuthState;

    QString mEmail;
    QString mPassword;
    QString mPassword2;
    QString mPassword3;

    QString mLastMessage;
    bool mHasCredentials;

    bool mStopRequests;

    bool mIsReady;
    bool mIsBusy;

    bool mFailed;

    std::unique_ptr<AuthRequest> mAuthRequest;

    CurrentUser(QObject *parent = 0);
    virtual ~CurrentUser();

    void registerUser();
    void verifyEmail();
    void requestConfirmation();
    void authenticateUser();
    void logoutUser();
    void requestPasswordRestore();
    void requestPasswordChange();

    void setReady(const bool ready);
    void setBusy(const bool busy);
    void setFailed(const bool failed);

    void setErrorMessage(const QString& errorMessage);
    void setLastMessage(const QString& lastMessage);

    void clearUser();
    void clearPasswords();

    QString password() const { return mPassword; }
    void setPassword(const QString& password);

    QString password2() const { return mPassword2; }
    void setPassword2(const QString& password);

    QString password3() const { return mPassword3; }
    void setPassword3(const QString& password);

    void loadUser();

};

#endif // CURRENTUSER_H

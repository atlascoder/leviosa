#ifndef CURRENTUSER_H
#define CURRENTUSER_H

#include <QObject>
#include <QString>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <QMutex>
#include <QFuture>
#include <QThread>

#include "aws/IdToken.h"
#include "aws/AccessToken.h"
#include "aws/RefreshToken.h"
#include "core/databasemanager.h"
#include "core/User.h"
#include "aws/CredentialsRequest.h"
#include "aws/AuthRequest.h"
#include <memory>

class CurrentUser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY isAuthenticatedChanged)

    User mUser;
    IdToken mIdToken;
    AccessToken mAccessToken;
    RefreshToken mRefreshToken;
    DatabaseManager& mDb;
    Aws::CognitoIdentity::Model::Credentials mCredentials;
    QMutex mCredentialsMutex;

    QFuture<void> mAuthResult;

    QString mLastMessage;
    bool mIsAuthenticated;
    bool mHasCredentials;

    bool mStopRequests;

    std::unique_ptr<AuthRequest> mAuthRequest;
    std::unique_ptr<CredentialsRequest> mCredentialsRequest;
public:
    static CurrentUser& instance();

    void fillCredentials(Aws::Auth::AWSCredentials & awsCredentials);

    // blocking credentials request
    void requestCredentials();

    QString email() const { return mUser.email(); }
    void setEmail(const QString& email);

    bool hasCredentials() const { return mHasCredentials; }

    bool isAuthenticated() const {return mIsAuthenticated; }

    void refreshTokens();

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
    void signUp(const QString& email, const QString& password);
    void signIn(const QString& email, const QString& password);
    void signOut();
    void restorePassword(const QString& email);
    void changePassword(const QString& newPassword, const QString& oldPassword);

signals:
    void authRequired();
    void credentialsReady();
    void credentialsRequestFailed(const QString& message);

    void signedUp();
    void signedIn();
    void signedOut();
    void passwordChanged();

    void authError(const QString& message);
    void restoreRequestSent();

    void emailChanged();
    void isAuthenticatedChanged();

private:
    CurrentUser(QObject *parent = 0);
    virtual ~CurrentUser();

    void registerUser(const QString& email, const QString& password);
    void authenticateUser(const QString& email, const QString& password);
    void logoutUser();
    void requestPasswordRestore(const QString& email);
    void requestPasswordChange(const QString& newPassword, const QString& password);

    void setAuthenticated(bool isAuthenticated);

};

#endif // CURRENTUSER_H

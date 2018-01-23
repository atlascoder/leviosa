#ifndef CURRENTUSER_H
#define CURRENTUSER_H

#include <QObject>
#include <QString>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <QMutex>
#include <QFuture>

#include "aws/IdToken.h"
#include "aws/AccessToken.h"
#include "aws/RefreshToken.h"
#include "core/databasemanager.h"
#include "core/User.h"
#include "aws/CredentialsRequest.h"

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

    QFuture<void> mCredentialsResult;
    QFuture<void> mAuthResult;

    QString mLastMessage;
    bool mIsAuthenticated;
    bool mHasCredentials;
public:
    static CurrentUser& instance();

    void askCredentials();
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

    void persistUserDataModified();

public slots:
    void signUp(const QString& email, const QString& password);
    void signIn(const QString& email, const QString& password);
    void signOut();
    void restorePassword();
    void changePassword(const QString& newPassword, const QString& oldPassword);

signals:
    void authRequired();
    void credentialsReady();
    void credentialsRequestFailed(const QString& message);

    void signedUp();
    void signedIn();
    void signedOut();
    void authError(const QString& message);
    void restoreRequestSent();

    void emailChanged();
    void isAuthenticatedChanged();

private:
    CurrentUser(QObject *parent = 0);

    void registerUser(const QString& email, const QString& password);
    void authenticateUser(const QString& email, const QString& password);
    void logoutUser();
    void requestPasswordRestore();
    void requestPasswordChange(const QString& newPassword, const QString& password);

    void setAuthenticated(bool isAuthenticated);

};

#endif // CURRENTUSER_H

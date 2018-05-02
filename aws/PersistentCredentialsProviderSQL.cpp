#include "PersistentCredentialsProviderSQL.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include <aws/core/utils/memory/stl/AWSMap.h>

#include "core/UserDAO.h"
#include "core/CredentialsDAO.h"
#include "core/DatabaseManager.h"

PersistentCredentialsProviderSQLite::PersistentCredentialsProviderSQLite(const Aws::String& identityPoolId, const Aws::String& accountId, bool disableCaching):
    m_identityPoolId(identityPoolId),
    m_accountId(accountId),
    m_disableCaching(disableCaching)
{
    if(!m_disableCaching) LoadData();
}

void PersistentCredentialsProviderSQLite::PersistIdentityId(const Aws::String & identityId)
{
    {
        std::lock_guard<std::mutex> locker(m_docMutex);
        m_identityId = identityId;
        DatabaseManager::instance().userDAO.persistCognitoIdentityId(QString::fromStdString(identityId.c_str()));
    }

    if (m_identityIdUpdatedCallback)
        m_identityIdUpdatedCallback(*this);
}

void PersistentCredentialsProviderSQLite::PersistLogins(const Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens> & logins)
{
    {
        std::lock_guard<std::mutex> locker(m_docMutex);
        m_logins = logins;

        QList<UserLogin> user_logins;
        for (auto& login : m_logins)
        {
            UserLogin login_node;
            login_node.login = login.first.c_str();
            login_node.accessToken = QString::fromStdString(login.second.accessToken.c_str());
            login_node.longtermToken = QString::fromStdString(login.second.longTermToken);
            login_node.longtermExpiration = login.second.longTermTokenExpiry;
            user_logins.append(login_node);
        }
        DatabaseManager::instance().credentialsDAO.persistLogins(user_logins);
    }

    if (m_loginsUpdatedCallback)
        m_loginsUpdatedCallback(*this);
}

void PersistentCredentialsProviderSQLite::LoadData()
{
    m_identityId = DatabaseManager::instance().userDAO.readCognitoIdentityId().toStdString();
    m_logins.clear();
    QList<UserLogin> user_logins = DatabaseManager::instance().credentialsDAO.readLogins();

    for (UserLogin l : user_logins) {
        Aws::Auth::LoginAccessTokens tokens;
        tokens.accessToken = l.accessToken.toStdString();
        tokens.longTermToken = l.longtermToken.toStdString();
        tokens.longTermTokenExpiry = l.longtermExpiration;
        m_logins[l.login.toStdString()] = tokens;
    }
}

bool PersistentCredentialsProviderSQLite::HasIdentityId() const
{
    if(m_disableCaching)
        return DatabaseManager::instance().userDAO.readCognitoIdentityId().isEmpty() > 0;
    else
        return !m_identityId.empty();
}

bool PersistentCredentialsProviderSQLite::HasLogins() const
{
    if(m_disableCaching)
        return DatabaseManager::instance().credentialsDAO.readLogins().size() > 0;
    else
        return !m_logins.empty();
}

Aws::String PersistentCredentialsProviderSQLite::GetIdentityId() const
{
    if(m_disableCaching)
        return DatabaseManager::instance().userDAO.readCognitoIdentityId().toStdString();
    else
        return m_identityId;
}

Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens> PersistentCredentialsProviderSQLite::GetLogins()
{
    if(m_disableCaching)
    {
        Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens> logins;
        QList<UserLogin> user_logins = DatabaseManager::instance().credentialsDAO.readLogins();

        for (UserLogin l : user_logins) {
            Aws::Auth::LoginAccessTokens tokens;
            tokens.accessToken = l.accessToken.toStdString();
            tokens.longTermToken = l.longtermToken.toStdString();
            tokens.longTermTokenExpiry = l.longtermExpiration;
            logins[l.login.toStdString()] = tokens;
        }
        return logins;
    }
    else
        return m_logins;
}


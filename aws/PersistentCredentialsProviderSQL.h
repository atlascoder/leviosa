#ifndef PERSISTENTCREDENTIALSPROVIDERSQL_H
#define PERSISTENTCREDENTIALSPROVIDERSQL_H

#include <aws/identity-management/auth/PersistentCognitoIdentityProvider.h>
#include <memory>

class PersistentCredentialsProviderSQLite : public Aws::Auth::PersistentCognitoIdentityProvider
{
public:
    PersistentCredentialsProviderSQLite(const Aws::String& identityPoolId, const Aws::String& accountId, bool disableCaching = false);

    bool HasIdentityId() const override;
    bool HasLogins() const override;
    Aws::String GetIdentityId() const override;
    Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens> GetLogins() override;
    Aws::String GetAccountId() const override { return m_accountId; }
    Aws::String GetIdentityPoolId() const override { return m_identityPoolId; }
    void PersistIdentityId(const Aws::String&) override;
    void PersistLogins(const Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens>&) override;

private:

    void PersistChanges() const;
    void LoadData();

    std::mutex m_docMutex;
    Aws::String m_identityPoolId;
    Aws::String m_accountId;
    Aws::String m_identityId;
    Aws::Map<Aws::String, Aws::Auth::LoginAccessTokens> m_logins;
    Aws::String m_identityFilePath;
    bool m_disableCaching;

    void initTable();
};

#endif // PERSISTENTCREDENTIALSPROVIDERSQL_H

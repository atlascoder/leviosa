#ifndef BASICDAO_H
#define BASICDAO_H

#include <QString>
#include <QString>
#include <QSqlQuery>
#include <memory>

class QSqlDatabase;

template <class T>
class BasicDAO
{
public:
    BasicDAO(QSqlDatabase & database) : mDatabase(database) {}
    void init() const;
    std::unique_ptr<std::vector<std::unique_ptr<T>>> items() const;
    void loadItems(std::vector<std::unique_ptr<T>>& receiver) const;
    void persistItem(T& item, bool notify) const;
    void destroy(T& item, bool notify) const;
    void clear(bool notify) const;
    int lastModified() const;
    std::unique_ptr<std::vector<std::unique_ptr<T>>> filtered(const QString& field, const QVariant & filter) const;
    void loadFiltered(std::vector<std::unique_ptr<T>>& receiver, const QString& field, const QVariant & filter) const;

    bool isSynced() const;
protected:
    QSqlDatabase & mDatabase;
private:
    static QString tableName();
    QString fieldsSQLDecl() const;
    std::unique_ptr<T> buildItem(QSqlQuery & query) const;
    void prepareFind(QSqlQuery & query, T& item) const;
    void prepareInsert(QSqlQuery & query, T& item) const;
    void prepareUpdate(QSqlQuery & query, T& item) const;

    void notifyDataChanged() const;
};

#include "BasicDAO.tpp"
#endif // BASICDAO_H

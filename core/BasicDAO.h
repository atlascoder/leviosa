#ifndef BASICDAO_H
#define BASICDAO_H

#include <QString>
#include <QSqlQuery>

class QSqlDatabase;

template <class T>
class BasicDAO
{

public:
    BasicDAO(QSqlDatabase & database) : mDatabase(database) {}
    void init() const;
    std::unique_ptr<std::vector<std::unique_ptr<T>>> items() const;
    void persistItem(T& item) const;
    void markDeleted(T& item) const;
    void destroy(T& item) const;
    void clear() const;
    int lastModified() const;

protected:
    QSqlDatabase & mDatabase;
private:
    static QString tableName();
    QString fieldsSQLDecl() const;
    std::unique_ptr<T> buildItem(QSqlQuery & query) const;
    void prepareFind(QSqlQuery & query, T& item) const;
    void prepareInsert(QSqlQuery & query, T& item) const;
    void prepareUpdate(QSqlQuery & query, T& item) const;
};

#include "BasicDAO.tpp"
#endif // BASICDAO_H

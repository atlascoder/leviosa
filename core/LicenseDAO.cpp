#include "LicenseDAO.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

#define TABLE_NAME "licensing"

void LicenseDAO::init() const
{
    if (!mDatabase.tables().contains(TABLE_NAME)) {
        QSqlQuery query(mDatabase);
        if (query.exec(
                    "CREATE TABLE " \
                    TABLE_NAME \
                    "(" \
                    "timestamp INTEGER" \
                    ")"
                    ))
            qDebug() << "Table " << TABLE_NAME << " was created";
        else
            qDebug() << "Table " << TABLE_NAME << " not created: " << query.lastError().text();
    }
}

void LicenseDAO::clear() const
{
    QSqlQuery query(mDatabase);
    if (query.exec("DELETE FROM "  TABLE_NAME))
        qDebug() << "Table " << TABLE_NAME << " cleared";
    else
        qDebug() << "Table " << TABLE_NAME << " not cleared: " << query.lastError().text();
}

bool LicenseDAO::agreed() const
{
    QSqlQuery query(mDatabase);
    if (query.exec("SELECT * FROM " TABLE_NAME))
        return query.next();
    else
        return false;
}

void LicenseDAO::persistAgree() const
{
    QSqlQuery query(mDatabase);
    uint now = QDateTime::currentDateTime().toTime_t();
    if (query.exec(
                "INSERT INTO " \
                TABLE_NAME \
                " (timestamp) " \
                "VALUES (" + QString::number(now) + ")"
            ))
        qDebug() << "License agree persisted";
    else
        qDebug() << "License agree not persisted: " << query.lastError().text();
}

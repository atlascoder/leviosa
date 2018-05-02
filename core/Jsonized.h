#ifndef JSONIZED_H
#define JSONIZED_H

#include <QJsonObject>

class Jsonized
{
public:
    virtual fromJsonObject(const QJsonObject& jsonObject) = 0;
    virtual fromJsonString(const QString& jsonString) = 0;
    QJsonObject toJsonObject() = 0;
    QString toJsonString() = 0;
};

#endif // JSONIZED_H

#ifndef LOCATION_H
#define LOCATION_H

#include <QObject>
#include <QString>

class Location : public QObject
{

    Q_OBJECT

    QString name;
    QString bss;
public:
    Location();
    Location(QString name, QString bss):
        name(name), bss(bss) {}
};

#endif // LOCATION_H

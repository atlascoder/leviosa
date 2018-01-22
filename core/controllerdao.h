#ifndef CONTROLLERDAO_H
#define CONTROLLERDAO_H
#include <QVector>
#include <memory>
#include "locationcontroller.h"

class QSqlDatabase;

class ControllerDao
{
    QSqlDatabase& mDatabase;
public:
    explicit ControllerDao(QSqlDatabase& database);
    void init() const;

    void addController(LocationController& coontroller) const;
    void addControllerInLocation(int locationId, LocationController& controller) const;

    void removeController(int id) const;
    void updateController(const LocationController& controller) const;
    void removeControllersInLocation(int locationId) const;
    std::unique_ptr<std::vector<std::unique_ptr<LocationController>>> controllersForLocation(int locationId) const;

    bool validateName(const QString& name, int locationId) const;
    bool validateMac(const QString& mac) const;
    int lastPosition(int locationId) const;

    void clear() const;
};

#endif // CONTROLLERDAO_H

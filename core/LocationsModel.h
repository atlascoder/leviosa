#ifndef LOCATIONSMODEL_H
#define LOCATIONSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QHash>
#include "Location.h"
#include "Controller.h"

using namespace std;

class LocationsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool single READ single NOTIFY singleChanged)

public:
    LocationsModel(QObject* parent = 0);

    enum Roles {
        UuidRole = Qt::UserRole + 1,
        NameRole,
        UtcOffsetRole,
        TimeZoneRole,
        PositionRole,
        BssidRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    bool single() const;

signals:
    void modelUpdated();

    void singleChanged();

public slots:
    void reloadData();
private:

    bool isIndexValid(const QModelIndex& index) const;
    shared_ptr<vector<shared_ptr<Location>>> mLocations;

};

#endif // LOCATIONSMODEL_H

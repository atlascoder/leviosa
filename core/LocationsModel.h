#ifndef LOCATIONSMODEL_H
#define LOCATIONSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QHash>
#include <QTimer>

#include "Location.h"
#include "Controller.h"

using namespace std;

class LocationsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool single READ single NOTIFY singleChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    LocationsModel(QObject* parent = 0);

    enum Roles {
        UuidRole = Qt::UserRole + 1,
        NameRole,
        TimeZoneRole,
        PositionRole,
        BssidRole,
        LocationTime
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    bool single() const;

signals:
    void modelUpdated();
    void singleChanged();
    void activeChanged();

public slots:
    void reloadData();

private slots:

    void onTimer();
private:
    bool mIsActive;
    QTimer* mTimer;

    bool active() const;
    void setActive(bool isActive);

    bool isIndexValid(const QModelIndex& index) const;
    shared_ptr<vector<shared_ptr<Location>>> mLocations;

    void updateTime();

};

#endif // LOCATIONSMODEL_H

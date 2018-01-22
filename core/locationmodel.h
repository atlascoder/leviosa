#ifndef LOCATIONMODEL_H
#define LOCATIONMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QHash>

#include"databasemanager.h"

class LocationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int selectedLocationId READ getSelectedLocationId WRITE setSelectedLocationId NOTIFY selectedLocationIdChanged)
    Q_PROPERTY(QString selectedLocationName READ getSelectedLocationName WRITE setSelectedLocationName NOTIFY selectedLocationNameChanged)
    Q_PROPERTY(QString selectedLocationSsid READ getSelectedLocationSsid WRITE setSelectedLocationSsid NOTIFY selectedLocationSsidChanged)
    Q_PROPERTY(QString selectedLocationBssid READ getSelectedLocationBssid WRITE setSelectedLocationBssid NOTIFY selectedLocationBssidChanged)
    Q_PROPERTY(int selectedLocationPosition READ getSelectedLocationPosition WRITE setSelectedLocationPosition NOTIFY selectedLocationPositionChanged)
    Q_PROPERTY(int selectedLocationUtcOffset READ getSelectedLocationUtcOffset WRITE setSelectedLocationUtcOffset NOTIFY selectedLocationUtcOffsetChanged)
public:
    LocationModel(QObject* parent = 0);
    virtual ~LocationModel() {}

    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        UtcOffsetRole,
        PositionRole,
        IsLocalRole,
        IsOnlineRole,
        SsidRole,
        BssidRole
    };

    QModelIndex addLocation(const UserLocation& location);
    Q_INVOKABLE int roleByName(const QString& name) const;
    Q_INVOKABLE void addLocationWithData(const QString& name, int utcOffset, const QString& bssid, const QString& ssid, int position);
    Q_INVOKABLE bool updateLocationWithData(const int id, const QString& name, int utcOffset, const QString& bssid, const QString& ssid, int position);
    Q_INVOKABLE QModelIndex findLocation(const int id) const;
    Q_INVOKABLE void fireDataChanged();
    Q_INVOKABLE QModelIndex indexOfRow(int row) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    int getSelectedLocationId() const;
    void setSelectedLocationId(int id);

    QString getSelectedLocationName() const;
    void setSelectedLocationName(const QString& name);

    QString getSelectedLocationBssid() const;
    void setSelectedLocationBssid(const QString& bssid);

    QString getSelectedLocationSsid() const;
    void setSelectedLocationSsid(const QString& ssid);

    int getSelectedLocationPosition() const;
    void setSelectedLocationPosition(int position);

    int getSelectedLocationUtcOffset() const;
    void setSelectedLocationUtcOffset(int utcOffset);

signals:
    void dataChanged(const QModelIndex& start, const QModelIndex& end);
    void selectedLocationIdChanged();
    void selectedLocationNameChanged();
    void selectedLocationSsidChanged();
    void selectedLocationBssidChanged();
    void selectedLocationPositionChanged();
    void selectedLocationUtcOffsetChanged();

private:

    bool isIndexValid(const QModelIndex& index) const;
    DatabaseManager& mDb;
    std::unique_ptr<std::vector<std::unique_ptr<UserLocation>>> mLocations;
    QPersistentModelIndex mSelectedLocationIndex;
};

#endif // LOCATIONMODEL_H

#ifndef LOCATIONMODEL_H
#define LOCATIONMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QHash>
#include "SyncableModel.h"

#include "databasemanager.h"

class LocationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString selectedLocationUuid READ getSelectedLocationUuid WRITE setSelectedLocationUuid NOTIFY selectedLocationUuidChanged)
    Q_PROPERTY(QString selectedLocationName READ getSelectedLocationName WRITE setSelectedLocationName NOTIFY selectedLocationNameChanged)
    Q_PROPERTY(QString selectedLocationBssid READ getSelectedLocationBssid WRITE setSelectedLocationBssid NOTIFY selectedLocationBssidChanged)
    Q_PROPERTY(int selectedLocationPosition READ getSelectedLocationPosition WRITE setSelectedLocationPosition NOTIFY selectedLocationPositionChanged)
    Q_PROPERTY(int selectedLocationUtcOffset READ getSelectedLocationUtcOffset WRITE setSelectedLocationUtcOffset NOTIFY selectedLocationUtcOffsetChanged)
    Q_PROPERTY(QString selectedTimezoneName READ getSelectedTimezoneName WRITE setSelectedTimezoneName NOTIFY selectedLocationUtcOffsetChanged)
    Q_PROPERTY(QString currentBssid READ currentBssid WRITE setCurrentBssid NOTIFY currentBssidChanged)

    Q_PROPERTY(QStringList positionOrder READ getPositionOrder NOTIFY positionOrderChanged)
public:
    LocationModel(QObject* parent = 0);
    virtual ~LocationModel() {}

    enum Roles {
        UuidRole = Qt::UserRole + 1,
        NameRole,
        UtcOffsetRole,
        PositionRole,
        IsOnWlanRole,
        IsOnlineRole,
        BssidRole
    };

    QModelIndex addLocation(const UserLocation& location);
    Q_INVOKABLE int roleByName(const QString& name) const;
    Q_INVOKABLE void addLocationWithData(const QString& name, int utcOffset, const QString& bssid);
    Q_INVOKABLE bool updateLocationWithData(const QString& uuid, const QString& name, int utcOffset, const QString& bssid, int position);
    Q_INVOKABLE QModelIndex findLocation(const QString& uuid) const;
    Q_INVOKABLE void fireDataChanged();
    Q_INVOKABLE QModelIndex indexOfRow(int row) const;
    Q_INVOKABLE void remove(const QString& uuid);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    QString getSelectedLocationUuid() const;
    void setSelectedLocationUuid(const QString& uuid);

    QString getSelectedLocationName() const;
    void setSelectedLocationName(const QString& name);

    QString getSelectedLocationBssid() const;
    void setSelectedLocationBssid(const QString& bssid);

    int getSelectedLocationPosition() const;
    void setSelectedLocationPosition(int position);

    int getSelectedLocationUtcOffset() const;
    void setSelectedLocationUtcOffset(int utcOffset);

    QString getSelectedTimezoneName() const;
    void setSelectedTimezoneName(const QString& timezoneName);

    QString currentBssid() const { return mCurrentBssid; }
    void setCurrentBssid(const QString& bssid);

    QStringList getPositionOrder() const;

signals:
    void selectedLocationUuidChanged();
    void selectedLocationNameChanged();
    void selectedLocationBssidChanged();
    void selectedLocationPositionChanged();
    void selectedLocationUtcOffsetChanged();
    void positionOrderChanged();
    void currentBssidChanged();

    void modelUpdated();

public slots:
    void updateModel();
private:

    bool isIndexValid(const QModelIndex& index) const;
    DatabaseManager& mDb;
    std::unique_ptr<std::vector<std::unique_ptr<UserLocation>>> mLocations;
    QPersistentModelIndex mSelectedLocationIndex;
    QString mCurrentBssid;
};

#endif // LOCATIONMODEL_H

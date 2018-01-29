#ifndef LOCATIONMODEL_H
#define LOCATIONMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QHash>
#include "location.h"

class LocationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString selectedLocationUuid READ getSelectedLocationUuid WRITE setSelectedLocationUuid NOTIFY selectedLocationUuidChanged)
    Q_PROPERTY(QString selectedLocationName READ getSelectedLocationName NOTIFY selectedLocationNameChanged)
    Q_PROPERTY(QString selectedLocationBssid READ getSelectedLocationBssid NOTIFY selectedLocationBssidChanged)
    Q_PROPERTY(int selectedLocationPosition READ getSelectedLocationPosition NOTIFY selectedLocationPositionChanged)
    Q_PROPERTY(int selectedLocationUtcOffset READ getSelectedLocationUtcOffset NOTIFY selectedLocationUtcOffsetChanged)
    Q_PROPERTY(QString selectedTimezoneName READ getSelectedTimezoneName NOTIFY selectedLocationUtcOffsetChanged)
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

    QModelIndex addLocation(const Location& location);
    Q_INVOKABLE int roleByName(const QString& name) const;
    Q_INVOKABLE void addLocationWithData(const QString& name, int utcOffset, const QString& bssid);
    Q_INVOKABLE void updateLocationWithData(const QString& uuid, const QString& name, int utcOffset, const QString& bssid, int position);
    Q_INVOKABLE QModelIndex findLocation(const QString& uuid) const;
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
    std::vector<std::unique_ptr<Location>>* mLocations;
    QPersistentModelIndex mSelectedLocationIndex;
    QString mCurrentBssid;
};

#endif // LOCATIONMODEL_H

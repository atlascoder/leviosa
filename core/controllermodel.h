#ifndef CONTROLLERMODEL_H
#define CONTROLLERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>
#include <QHash>

#include "databasemanager.h"
#include "locationmodel.h"
#include "WlanAPI.h"
#include "WanAPI.h"

class ControllerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString locationUuid READ locationUuid WRITE setLocationUuid)
    Q_PROPERTY(QString locationName READ getLocationName NOTIFY locationNameChanged)
    Q_PROPERTY(QString selectedControllerMac READ getSelectedControllerMac WRITE setSelectedControllerMac NOTIFY selectedControllerMacChanged)
    Q_PROPERTY(QString selectedControllerName READ getSelectedControllerName WRITE setSelectedControllerName NOTIFY selectedControllerNameChanged)
    Q_PROPERTY(int selectedControllerPosition READ getSelectedControllerPosition WRITE setSelectedControllerPosition NOTIFY selectedControllerPositionChanged)
    Q_PROPERTY(int selectedControllerOpenAt READ getSelectedControllerOpenAt WRITE setSelectedControllerOpenAt NOTIFY selectedControllerOpenAtChanged)
    Q_PROPERTY(int selectedControllerCloseAt READ getSelectedControllerCloseAt WRITE setSelectedControllerCloseAt NOTIFY selectedControllerCloseAtChanged)
    Q_PROPERTY(int selectedControllerDays READ getSelectedControllerDays WRITE setSelectedControllerDays NOTIFY selectedControllerDaysChanged)
    Q_PROPERTY(QStringList positionOrder READ getPositionOrder NOTIFY positionOrderChanged)
    Q_PROPERTY(QString selectedControllerIp READ getSelectedControllerIp WRITE setSelectedControllerIp NOTIFY selectedControllerIpChanged)

    Q_PROPERTY(bool isOnWlan READ getOnWlan WRITE setOnWlan NOTIFY onWlanChanged)

public:
    ControllerModel(QObject* parent = 0);
    virtual ~ControllerModel();
    enum Roles {
        MacRole = Qt::UserRole + 1,
        LocationUuid,
        NameRole,
        PositionRole,
        OpenAtRole,
        CloseAtRole,
        DaysRole,
        IPRole,
        IsOnWlanRole
    };

    QModelIndex addController(const LocationController& controller);
    Q_INVOKABLE QModelIndex findController(const QString& mac);
    Q_INVOKABLE void addControllerWithMacAndIP(const QString &mac, const QString &ip);
    Q_INVOKABLE void updateControllerWithData(const QString& mac, const QString& name, int position);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int roleByName(const QString& name) const;
    Q_INVOKABLE QModelIndex indexOfRow(int row) const;

    Q_INVOKABLE void setLocation(const QString& locationUuid);
    Q_INVOKABLE QString getFreeName();

    Q_INVOKABLE void shadeCmd(const QString& mac, char channel, int cmd);

    QString locationUuid() const;
    void setLocationUuid(const QString& locationUuid);

signals:
    void locationNameChanged();
    void selectedControllerMacChanged();
    void selectedControllerNameChanged();
    void selectedControllerPositionChanged();
    void selectedControllerOpenAtChanged();
    void selectedControllerCloseAtChanged();
    void selectedControllerDaysChanged();
    void selectedControllerIpChanged();
    void positionOrderChanged();

    void onWlanChanged();

private:

    bool isIndexValid(const QModelIndex& index) const;

    void loadControllers(const QString& mac);

    DatabaseManager& mDb;
    bool mIsOnWlan;
    WlanAPI mWlanAPI;
    WanAPI mWanAPI;
    QString mLocationUuid;

    std::unique_ptr<std::vector<std::unique_ptr<LocationController>>> mControllers;

    QPersistentModelIndex mSelectedControllerIndex;

    QString getLocationName() const;

    QString getSelectedControllerMac() const;
    void setSelectedControllerMac(const QString& mac);

    QString getSelectedControllerName() const;
    void setSelectedControllerName(const QString& name);

    int getSelectedControllerPosition() const;
    void setSelectedControllerPosition(int position);

    int getSelectedControllerOpenAt() const;
    void setSelectedControllerOpenAt(int openAt);

    int getSelectedControllerCloseAt() const;
    void setSelectedControllerCloseAt(int closeAt);

    int getSelectedControllerDays() const;
    void setSelectedControllerDays(int days);

    QString getSelectedControllerIp() const;
    void setSelectedControllerIp(const QString &ip);

    QStringList getPositionOrder() const;

    bool getOnWlan() const;
    void setOnWlan(bool isOnWlan);

};

#endif // CONTROLLERMODEL_H

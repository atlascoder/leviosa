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
    Q_PROPERTY(int locationId READ locationId WRITE setLocationId)
    Q_PROPERTY(QString locationName READ getLocationName NOTIFY locationNameChanged)
    Q_PROPERTY(int selectedControllerId READ getSelectedControllerId WRITE setSelectedControllerId NOTIFY selectedControllerIdChanged)
    Q_PROPERTY(QString selectedControllerName READ getSelectedControllerName WRITE setSelectedControllerName NOTIFY selectedControllerNameChanged)
    Q_PROPERTY(QString selectedControllerMac READ getSelectedControllerMac WRITE setSelectedControllerMac NOTIFY selectedControllerMacChanged)
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
        IdRole = Qt::UserRole + 1,
        NameRole,
        MacRole,
        PositionRole,
        OpenAtRole,
        CloseAtRole,
        DaysRole,
        IPRole,
        IsOnWlanRole
    };

    QModelIndex addController(const LocationController& controller);
    Q_INVOKABLE QModelIndex findController(int id);
    Q_INVOKABLE void addControllerWithMacAndIP(const QString &mac, const QString &ip);
    Q_INVOKABLE void updateControllerWithData(int id, const QString& name, int position);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int roleByName(const QString& name) const;
    Q_INVOKABLE QModelIndex indexOfRow(int row) const;

    Q_INVOKABLE void setLocation(int locationId);
    Q_INVOKABLE QString getFreeName();

    Q_INVOKABLE void shadeCmd(int controllerId, char channel, int cmd);

    int locationId() const;
    void setLocationId(int locationId);

signals:
    void locationNameChanged();
    void selectedControllerIdChanged();
    void selectedControllerNameChanged();
    void selectedControllerMacChanged();
    void selectedControllerPositionChanged();
    void selectedControllerOpenAtChanged();
    void selectedControllerCloseAtChanged();
    void selectedControllerDaysChanged();
    void selectedControllerIpChanged();
    void positionOrderChanged();

    void onWlanChanged();

private:

    bool isIndexValid(const QModelIndex& index) const;

    void loadControllers(int locationId);

    DatabaseManager& mDb;
    bool mIsOnWlan;
    WlanAPI mWlanAPI;
    WanAPI mWanAPI;
    int mLocationId;

    std::unique_ptr<std::vector<std::unique_ptr<LocationController>>> mControllers;

    QPersistentModelIndex mSelectedControllerIndex;

    QString getLocationName() const;

    int getSelectedControllerId() const;
    void setSelectedControllerId(int controllerId);

    QString getSelectedControllerName() const;
    void setSelectedControllerName(const QString& name);

    QString getSelectedControllerMac() const;
    void setSelectedControllerMac(const QString& mac);

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

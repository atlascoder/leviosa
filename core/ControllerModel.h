#ifndef CONTROLLERMODEL_H
#define CONTROLLERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>
#include <QHash>
#include <QMutex>
#include <QMap>

#include "Controller.h"
#include "ControllerAPI.h"
#include "WanAPI.h"

using namespace std;

class ControllerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString locationUuid READ locationUuid WRITE setLocationUuid NOTIFY locationUuidChanged)
    Q_PROPERTY(QString locationName READ getLocationName NOTIFY locationNameChanged)
    Q_PROPERTY(QString locationBssid READ locationBssid NOTIFY locationBssidChanged)

    Q_PROPERTY(QString selectedControllerMac READ getSelectedControllerMac WRITE setSelectedControllerMac NOTIFY selectedControllerMacChanged)
    Q_PROPERTY(QString selectedControllerName READ getSelectedControllerName WRITE setSelectedControllerName NOTIFY selectedControllerNameChanged)
    Q_PROPERTY(int selectedControllerPosition READ getSelectedControllerPosition WRITE setSelectedControllerPosition NOTIFY selectedControllerPositionChanged)
    Q_PROPERTY(int selectedControllerOpenAt READ getSelectedControllerOpenAt WRITE setSelectedControllerOpenAt NOTIFY selectedControllerOpenAtChanged)
    Q_PROPERTY(int selectedControllerCloseAt READ getSelectedControllerCloseAt WRITE setSelectedControllerCloseAt NOTIFY selectedControllerCloseAtChanged)
    Q_PROPERTY(int selectedControllerDays READ getSelectedControllerDays WRITE setSelectedControllerDays NOTIFY selectedControllerDaysChanged)
    Q_PROPERTY(QStringList positionOrder READ getPositionOrder NOTIFY positionOrderChanged)
    Q_PROPERTY(QString selectedControllerIp READ getSelectedControllerIp WRITE setSelectedControllerIp NOTIFY selectedControllerIpChanged)

    Q_PROPERTY(QString selecetedControllerStatus READ selectedControllerStatus NOTIFY selectedControllerStatusChanged)

    Q_PROPERTY(int selectedControllerState READ selectedControllerState NOTIFY selectedControllerStatusChanged)
    Q_PROPERTY(QString currentBssid READ currentBssid WRITE setCurrentBssid NOTIFY currentBssidChanged)

    Q_PROPERTY(bool isOnWlan READ getOnWlan WRITE setOnWlan NOTIFY onWlanChanged )
    Q_PROPERTY(bool isCurrentLocation READ isCurrentLocation NOTIFY locationStatusChanged )
    Q_PROPERTY(bool isNewLocation READ isNewLocation NOTIFY locationStatusChanged )

    Q_PROPERTY(QString locationStatusText READ locationStatusText NOTIFY locationStatusChanged)

    Q_PROPERTY(bool discovering READ discovering WRITE setDiscovering NOTIFY discoveringChanged)

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    Q_PROPERTY(bool dataLoaded READ dataLoaded NOTIFY dataLoadedChanged)

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
        IsOnWlanRole,
        ControllerStateRole
    };


    QModelIndex addController(const Controller& controller);
    Q_INVOKABLE QModelIndex findController(const QString& mac);
//    Q_INVOKABLE void addControllersFromList(const QString& uuid, const QString& list);
    Q_INVOKABLE void addControllerWithMacAndIP(const QString& uuid, const QString &mac, const QString &ip);
    Q_INVOKABLE void updateControllerWithData(const QString& mac, const QString& name, int position);
    Q_INVOKABLE QString macByIndex(int idx) const;
    Q_INVOKABLE QString nameByIndex(int idx) const;

    Q_INVOKABLE void setSelectedControllerIndex(int index);
    Q_INVOKABLE void checkIP(const QString& mac, const QString& ip);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int roleByName(const QString& name) const;
    Q_INVOKABLE QModelIndex indexOfRow(int row) const;

    Q_INVOKABLE void remove(const QString& mac);

    Q_INVOKABLE void commandShade(const QString& mac, int channel, int command);

    Q_INVOKABLE QString ipByMac(const QString& mac);

    Q_INVOKABLE QString getFreeName();

    Q_INVOKABLE void searchController(const QString& mac);
    QString getFreeName(const shared_ptr<vector<shared_ptr<Controller>>>& newItems);

    QString locationUuid() const;
    void setLocationUuid(const QString& locationUuid);    

signals:
    void locationUuidChanged();
    void locationNameChanged();
    void selectedControllerMacChanged();
    void selectedControllerNameChanged();
    void selectedControllerPositionChanged();
    void selectedControllerOpenAtChanged();
    void selectedControllerCloseAtChanged();
    void selectedControllerDaysChanged();
    void selectedControllerIpChanged();
    void positionOrderChanged();
    void countChanged();

    void currentBssidChanged();
    void locationBssidChanged();

    void locationStatusChanged();

    void onWlanChanged();

    void discoveringChanged();

    void selectedControllerStatusChanged();

    void dataLoadedChanged();

public slots:
    void updateModel();
    void controllerStatusUpdated(const QString& mac);

private:

    bool isIndexValid(const QModelIndex& index) const;

    void loadControllers(const QString& mac);

    bool mIsOnWlan;
    QString mLocationUuid;
    QString mCurrentIp;
    shared_ptr<vector<shared_ptr<Controller>>> mControllers;
    QPersistentModelIndex mSelectedIndex;
    QString mCurrentBssid;
    QString mLocationBssid;

    unique_ptr<map<QString, shared_ptr<ControllerAPI>>> mAPIs;

    bool mControllerTimeMismatch;
    bool mDiscovering;
    bool mIsDataLoaded;

    QMutex mUpdatingMtx;

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

    QString selectedControllerStatus();

    int selectedControllerState() const;

    QString currentBssid() const { return mCurrentBssid; }
    void setCurrentBssid(const QString& currentBssid);

    QString locationBssid() const { return mLocationBssid; }
    void setLocationBssid(const QString& locationBssid);

    bool getOnWlan() const;
    void setOnWlan(bool isOnWlan);

    bool isCurrentLocation() const;
    bool isNewLocation() const;

    bool discovering() const { return mDiscovering; }
    void setDiscovering(bool discovering);

    bool dataLoaded() const { return mIsDataLoaded; }
    void setDataLoaded(bool isLoaded);

    void checkConfiguration(const shared_ptr<ControllerAPI>& controller);

    QString locationStatusText() const;

    void setControllersAPI();

};

#endif // CONTROLLERMODEL_H

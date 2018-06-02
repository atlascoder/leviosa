#ifndef ZONEMODEL_H
#define ZONEMODEL_H

#include <QAbstractItemModel>
#include <QVariant>
#include <QHash>
#include <QByteArray>
#include <memory>

#include "UserData.h"
#include "ControllerAPI.h"
#include "Controller.h"
#include "ShadeGroup.h"

using namespace std;

class ZoneModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY controllerChanged)
    Q_PROPERTY(QString name READ name WRITE setName  NOTIFY nameChanged)
    Q_PROPERTY(QString title READ title NOTIFY nameChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QStringList positionOrder READ positionOrder NOTIFY controllerChanged)
    Q_PROPERTY(QString locationUuid READ locationUuid NOTIFY controllerChanged)
    Q_PROPERTY(QString locationBssid READ locationBssid NOTIFY controllerChanged)
    Q_PROPERTY(QString controllerMac READ controllerMac NOTIFY controllerChanged)
    Q_PROPERTY(QString timezone READ timezone NOTIFY timezoneChanged)
    Q_PROPERTY(QString controllerStatus READ controllerStatus NOTIFY controllerStatusChanged)
    Q_PROPERTY(bool single READ single NOTIFY controllerChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY shadeGroupsChanged)
public:
    ZoneModel(QObject *parent = nullptr);

    enum Roles {
        UuidRole = Qt::UserRole + 1,
        ChannelRole,
        NameRole,
        PositionRole,
        OpenAtRole,
        CloseAtRole,
        DaysRole,
        OpenAtTextRole,
        CloseAtTextRole,
        ShadeStateRole,
        OpenAtUSRole,
        CloseAtUSRole
    };

    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void deleteZone();

    Q_INVOKABLE void reloadShadeGroups();
    Q_INVOKABLE void commandController(const QString& groupUuid, int command);
    Q_INVOKABLE void resetControls();
signals:
    void controllerChanged();
    void nameChanged();
    void positionChanged();
    void controllerStatusChanged();
    void shadeGroupsChanged();
    void timezoneChanged();

private slots:
    void controllerStateChanged(const QString& mac);
private:

    shared_ptr<Controller> mController;
    shared_ptr<vector<shared_ptr<Controller>>> mNeighbours;
    shared_ptr<vector<shared_ptr<ShadeGroup>>> mShadeGroups;
    shared_ptr<ControllerAPI> mControllerAPI;
    QString mBssid;
    int mOldPosition;

    bool isIndexValid(const QModelIndex& index) const;

    QString uuid() const;
    void setUuid(const QString& uuid);

    QString name() const;
    void setName(const QString& name);

    int position() const;
    void setPosition(const int position);

    QStringList positionOrder() const;

    QString title() const;
    QString locationUuid() const;
    QString locationBssid() const;
    QString timezone() const;
    QString controllerMac() const;
    QString controllerStatus() const;
    bool single() const;

};

#endif // ZONEMODEL_H

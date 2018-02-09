#ifndef SHADEGROUPMODEL_H
#define SHADEGROUPMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "databasemanager.h"
#include "controller.h"
#include "controllermodel.h"
#include "ControllerAPI.h"

class ShadeGroupModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString controllerMac READ controllerMac WRITE setControllerMac NOTIFY controllerMacChanged)
    Q_PROPERTY(QString selectedGroupName READ getSelectedGroupName WRITE setSelectedGroupName NOTIFY selectedGroupNameChanged)
    Q_PROPERTY(int selectedGroupPosition READ getSelectedGroupPosition NOTIFY selectedGroupPositionChanged)
    Q_PROPERTY(int selectedGroupOpenAt READ getSelectedGroupOpenAt WRITE setSelectedGroupOpenAt NOTIFY selectedGroupOpenAtChanged)
    Q_PROPERTY(int selectedGroupCloseAt READ getSelectedGroupCloseAt WRITE setSelectedGroupCloseAt NOTIFY selectedGroupCloseAtChanged)
    Q_PROPERTY(int selectedGroupDays READ getSelectedGroupDays WRITE setSelectedGroupDays NOTIFY selectedGroupDaysChanged)

    Q_PROPERTY(QString selectedOpenAtText READ getSelectedOpenAtText NOTIFY selectedGroupOpenAtChanged)
    Q_PROPERTY(QString selectedCloseAtText READ getSelectedCloseAtText NOTIFY selectedGroupCloseAtChanged)

    Q_PROPERTY(float selectedOpenAtUS READ getSelectedOpenAtUS WRITE setSelectedOpenAtUS NOTIFY selectedGroupOpenAtChanged)
    Q_PROPERTY(float selectedCloseAtUS READ getSelectedCloseAtUS WRITE setSelectedCloseAtUS NOTIFY selectedGroupCloseAtChanged)

    Q_PROPERTY(int selectedChannel READ getSelectedChannel WRITE setSelectedChannel NOTIFY selectedChannelChanged)

    Q_PROPERTY(Shade::ShadeState selectedShadeState READ getSelectedShadeState WRITE setSelectedShadeState NOTIFY selectedShadeStateChanged)

    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)

    Q_PROPERTY(QStringList positionOrder READ getPositionOrder NOTIFY positionOrderChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY controllerMacChanged)

public:
    ShadeGroupModel(QObject* parent = 0);
    enum Roles {
        ChannelRole = Qt::UserRole + 1,
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

    QModelIndex addShadeGroup(const ShadeGroup& shadesGroup);
    Q_INVOKABLE void addShadesGroupWithData(const QString& controllerMac, const QString& name);
    Q_INVOKABLE void updateShadeGroupsWithData(int channel, const QString& name, int position, float openAtUS, float closeAtUS, int days, bool scheduleForController);

    Q_INVOKABLE void removeShadeGroup(const QString& mac, char channel);
    Q_INVOKABLE int roleByName(const QString& roleName) const;

    Q_INVOKABLE void setStateToAll(const Shade::ShadeState state);
    Q_INVOKABLE void setStateToAll(const int state);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex find(int channel) const;

    QString controllerMac() const;
    void setControllerMac(const QString& mac);

signals:
    void controllerMacChanged();
    void selectedChannelChanged();
    void selectedGroupNameChanged();
    void selectedGroupPositionChanged();
    void selectedGroupOpenAtChanged();
    void selectedGroupCloseAtChanged();
    void selectedGroupDaysChanged();
    void selectedShadeStateChanged();
    void positionOrderChanged();

    void ipAddressChanged();

    void scheduleSet();
    void scheduleSetFailed();

public slots:
    void updateModel();

private slots:
    void setScheduleToController();
    void onScheduleSet();
private:

    DatabaseManager& mDb;
    QString mControllerMac;
    QPersistentModelIndex mSelectedIndex;
    std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> mShadeGroups;
    std::unique_ptr<ControllerAPI> mControllerAPI;
    QString mIpAddress;

    int getSelectedChannel() const;
    void setSelectedChannel(int channel);

    QString getSelectedGroupName() const;
    void setSelectedGroupName(const QString& name);

    int getSelectedGroupPosition() const;

    int getSelectedGroupOpenAt() const;
    void setSelectedGroupOpenAt(int openAt);

    int getSelectedGroupCloseAt() const;
    void setSelectedGroupCloseAt(int closeAt);

    int getSelectedGroupDays() const;
    void setSelectedGroupDays(int days);

    QString getSelectedOpenAtText() const;
    QString getSelectedCloseAtText() const;

    float getSelectedOpenAtUS() const;
    void setSelectedOpenAtUS(const float openAtUS);

    float getSelectedCloseAtUS() const;
    void setSelectedCloseAtUS(const float closeAtUS);

    Shade::ShadeState getSelectedShadeState() const;
    void setSelectedShadeState(Shade::ShadeState state);

    QString ipAddress() const { return mIpAddress; }
    void setIpAddress(const QString& ipAddress);

    bool isIndexValid(const QModelIndex& index) const;

    void loadShadeGroups();

    QStringList getPositionOrder() const;

    char findFreeChannel(const QString& constrollerMac) const;    

};

#endif // SHADEGROUPMODEL_H

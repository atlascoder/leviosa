#ifndef SHADEGROUPMODEL_H
#define SHADEGROUPMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "databasemanager.h"
#include "controller.h"
#include "controllermodel.h"

class ShadeGroupModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString controllerMac READ controllerMac WRITE setControllerMac NOTIFY controllerMacChanged)
    Q_PROPERTY(QString selectedGroupName READ getSelectedGroupName WRITE setSelectedGroupName NOTIFY selectedGroupNameChanged)
    Q_PROPERTY(int selectedGroupPosition READ getSelectedGroupPosition WRITE setSelectedGroupPosition NOTIFY selectedGroupPositionChanged)
    Q_PROPERTY(int selectedGroupOpenAt READ getSelectedGroupOpenAt WRITE setSelectedGroupOpenAt NOTIFY selectedGroupOpenAtChanged)
    Q_PROPERTY(int selectedGroupCloseAt READ getSelectedGroupCloseAt WRITE setSelectedGroupCloseAt NOTIFY selectedGroupCloseAtChanged)
    Q_PROPERTY(int selectedGroupDays READ getSelectedGroupDays WRITE setSelectedGroupDays NOTIFY selectedGroupDaysChanged)

    Q_PROPERTY(QString selectedOpenAtText READ getSelectedOpenAtText NOTIFY selectedGroupOpenAtChanged)
    Q_PROPERTY(QString selectedCloseAtText READ getSelectedCloseAtText NOTIFY selectedGroupCloseAtChanged)

    Q_PROPERTY(float selectedOpenAtUS READ getSelectedOpenAtUS WRITE setSelectedOpenAtUS NOTIFY selectedGroupOpenAtChanged)
    Q_PROPERTY(float selectedCloseAtUS READ getSelectedCloseAtUS WRITE setSelectedCloseAtUS NOTIFY selectedGroupCloseAtChanged)

    Q_PROPERTY(int selectedChannel READ getSelectedChannel WRITE setSelectedChannel NOTIFY selectedChannelChanged)

    Q_PROPERTY(Shade::ShadeState selectedShadeState READ getSelectedShadeState WRITE setSelectedShadeState NOTIFY selectedShadeStateChanged)

    Q_PROPERTY(QStringList positionOrder READ getPositionOrder NOTIFY positionOrderChanged)

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
    Q_INVOKABLE void removeShadeGroup(char channel);
    Q_INVOKABLE int roleByName(const QString& roleName) const;

    Q_INVOKABLE void setStateToAll(const Shade::ShadeState state);
    Q_INVOKABLE void setStateToAll(const int state);

    Q_INVOKABLE void setScheduleForCurrentController(int days, float openAtUS, float closeAtUS);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

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
private:

    int getSelectedChannel() const;
    void setSelectedChannel(int channel);

    QString getSelectedGroupName() const;
    void setSelectedGroupName(const QString& name);

    int getSelectedGroupPosition() const;
    void setSelectedGroupPosition(int position);

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

    bool isIndexValid(const QModelIndex& index) const;

    void loadGroupsShades(const QString& controllerMac);

    QStringList getPositionOrder() const;

    char findFreeChannel(const QString& constrollerMac) const;

    DatabaseManager& mDb;
    QString mControllerMac;
    QPersistentModelIndex mSelectedIndex;
    std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> mShadeGroups;


};

#endif // SHADEGROUPMODEL_H

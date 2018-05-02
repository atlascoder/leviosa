#ifndef LOCATIONMODEL_H
#define LOCATIONMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QHash>

#include <queue>

#include "Location.h"
#include "Controller.h"

using namespace std;

class LocationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString bssid READ bssid WRITE setBssid NOTIFY bssidChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int utcOffset READ utcOffset WRITE setUtcOffset NOTIFY utcOffsetChanged)
    Q_PROPERTY(QString timezoneName READ timezoneName NOTIFY utcOffsetChanged)
    Q_PROPERTY(QStringList positionOrder READ getPositionOrder NOTIFY zonesChanged)
    Q_PROPERTY(bool isNew READ isNew NOTIFY isNewChanged)
    Q_PROPERTY(bool single READ single NOTIFY zonesChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY zonesChanged)

public:
    LocationModel(QObject* parent = 0);

    enum Roles {
        UuidRole = Qt::UserRole + 1,
        NameRole,
        UtcOffsetRole,
        PositionRole,
        ControllerStateRole
    };

    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void deleteLocation();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    QString uuid() const;
    void setUuid(const QString& uuid);

    QString name() const;
    void setName(const QString& name);

    QString bssid() const;
    void setBssid(const QString& bssid);

    int position() const;
    void setPosition(int position);

    int utcOffset() const;
    void setUtcOffset(int utcOffset);

    QString timezoneName() const;

    bool single() const;

    int count() const;

    QStringList getPositionOrder() const;

signals:
    void uuidChanged();
    void nameChanged();
    void bssidChanged();
    void positionChanged();
    void utcOffsetChanged();
    void currentBssidChanged();
    void isNewChanged();
    void zonesChanged();

    void modelUpdated();

public slots:
    void reloadData();

    void commandShade(const int command);

private slots:
    void checkControllers();

private:

    bool isIndexValid(const QModelIndex& index) const;
    shared_ptr<Location> mLocation;
    bool mIsNew;
    shared_ptr<vector<shared_ptr<Location>>> mNeighbours;
    int mOldPosition;
    shared_ptr<vector<shared_ptr<Controller>>> mControllers;

    unique_ptr<queue<pair<int, int>>> mCommandsQueue;

    static QString utcOffsetToTimezone(int utcOffset);

    void commandAwsLocation(const int command);

    bool isNew() const;
    void setNew(const bool isNew);

};

#endif // LOCATIONMODEL_H

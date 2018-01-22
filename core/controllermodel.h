#ifndef CONTROLLERMODEL_H
#define CONTROLLERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>
#include <QHash>

#include "databasemanager.h"
#include "locationmodel.h"

class ControllerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int locationId READ locationId WRITE setLocationId)
    Q_PROPERTY(QString locationName READ getLocationName)
    Q_PROPERTY(int selectedControllerId READ getSelectedControllerId WRITE setSelectedControllerId NOTIFY selectedControllerIdChanged)
    Q_PROPERTY(QString selectedControllerName READ getSelectedControllerName WRITE setSelectedControllerName NOTIFY selectedControllerNameChanged)
    Q_PROPERTY(QString selectedControllerMac READ getSelectedControllerMac WRITE setSelectedControllerMac NOTIFY selectedControllerMacChanged)
    Q_PROPERTY(int selectedControllerPosition READ getSelectedControllerPosition WRITE setSelectedControllerPosition NOTIFY selectedControllerPositionChanged)

public:
    ControllerModel(QObject* parent = 0);
    virtual ~ControllerModel() {}
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        MacRole,
        PositionRole
    };

    QModelIndex addController(const LocationController& controller);
    Q_INVOKABLE QModelIndex findController(int id);
    Q_INVOKABLE void addControllerWithMac(const QString& mac);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int roleByName(const QString& name) const;
    Q_INVOKABLE QModelIndex indexOfRow(int row) const;

    Q_INVOKABLE void setLocation(int locationId);
    Q_INVOKABLE QString getFreeName();

    int locationId() const;
    void setLocationId(int locationId);

signals:
    void selectedControllerIdChanged();
    void selectedControllerNameChanged();
    void selectedControllerMacChanged();
    void selectedControllerPositionChanged();
private:

    bool isIndexValid(const QModelIndex& index) const;

    void loadControllers(int locationId);

    DatabaseManager& mDb;
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

};

#endif // CONTROLLERMODEL_H

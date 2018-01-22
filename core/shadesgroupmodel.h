#ifndef SHADESGROUPMODEL_H
#define SHADESGROUPMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "databasemanager.h"
#include "locationcontroller.h"
#include "controllermodel.h"

class ShadesGroupModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int controllerId READ controllerId WRITE setControllerId)
public:
    ShadesGroupModel(QObject* parent = 0);
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        PositionRole
    };

    QModelIndex addShadeGroup(const ShadesGroup& shadesGroup);
    Q_INVOKABLE void addShadesGroupWithData(int controllerId, const QString& name, int position = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    int controllerId() const;
    void setControllerId(int controllerId);

    void setController(int controllerId);

private:

    bool isIndexValid(const QModelIndex& index) const;

    void loadGroupsShades(int controllerId);

    DatabaseManager& mDb;
    int mControllerId;
    std::unique_ptr<std::vector<std::unique_ptr<ShadesGroup>>> mShadesGroups;


};

#endif // SHADESGROUPMODEL_H

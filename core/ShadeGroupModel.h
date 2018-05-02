#ifndef SHADEGROUPMODEL_H
#define SHADEGROUPMODEL_H

#include <QObject>
#include<QStringList>
#include "ShadeGroup.h"

using namespace std;

class ShadeGroupModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString controllerMac READ controllerMac WRITE setControllerMac NOTIFY controllerMacChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int openAt READ openAt WRITE setOpenAt NOTIFY openAtChanged)
    Q_PROPERTY(int closeAt READ closeAt WRITE setCloseAt NOTIFY closeAtChanged)
    Q_PROPERTY(int days READ days WRITE setDays NOTIFY daysChanged)
    Q_PROPERTY(QString openAtText READ openAtText NOTIFY openAtChanged)
    Q_PROPERTY(QString closeAtText READ closeAtText NOTIFY closeAtChanged)
    Q_PROPERTY(float openAtUS READ openAtUS WRITE setOpenAtUS NOTIFY openAtChanged)
    Q_PROPERTY(float closeAtUS READ closeAtUS WRITE setCloseAtUS NOTIFY closeAtChanged)
    Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(Shade::ShadeState shadeState READ shadeState WRITE setShadeState NOTIFY shadeStateChanged)
    Q_PROPERTY(QStringList positionOrder READ getPositionOrder NOTIFY positionOrderChanged)
    Q_PROPERTY(bool isNew READ isNew NOTIFY isNewChanged)
    Q_PROPERTY(bool forAll READ isForAll WRITE setForAll NOTIFY forAllChanged)

public:
    ShadeGroupModel(QObject* parent = 0);

    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void deleteGroup();

signals:
    void uuidChanged();
    void controllerMacChanged();
    void channelChanged();
    void nameChanged();
    void positionChanged();
    void openAtChanged();
    void closeAtChanged();
    void daysChanged();
    void shadeStateChanged();
    void positionOrderChanged();
    void isNewChanged();
    void forAllChanged();
private:

    shared_ptr<ShadeGroup> mShadeGroup;
    bool mIsNew;
    shared_ptr<vector<shared_ptr<ShadeGroup>>> mNeighbours;
    int mOldPosition;
    bool mForAll;

    QString uuid() const;
    void setUuid(const QString& uuid);

    QString controllerMac() const;
    void setControllerMac(const QString& mac);

    int channel() const;
    void setChannel(int channel);

    QString name() const;
    void setName(const QString& name);

    int position() const;
    void setPosition(int position);

    int openAt() const;
    void setOpenAt(int openAt);

    int closeAt() const;
    void setCloseAt(int closeAt);

    int days() const;
    void setDays(int days);

    QString openAtText() const;
    QString closeAtText() const;

    float openAtUS() const;
    void setOpenAtUS(const float openAtUS);

    float closeAtUS() const;
    void setCloseAtUS(const float closeAtUS);

    bool isNew() const;
    void setNew(const bool isNew);

    bool isForAll() const { return mForAll; }
    void setForAll(const bool forAll);

    Shade::ShadeState shadeState() const;
    void setShadeState(Shade::ShadeState state);

    QStringList getPositionOrder() const;

    char findFreeChannel() const;

};

#endif // SHADEGROUPMODEL_H

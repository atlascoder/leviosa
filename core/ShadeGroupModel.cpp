#include "ShadeGroupModel.h"

#include <QUuid>

#include "UserData.h"
#include "ControllerAPI.h"
#include "ControllerConnectionsManager.h"

using namespace std;

ShadeGroupModel::ShadeGroupModel(QObject* parent):
    QObject(parent),
    mShadeGroup(new ShadeGroup),
    mIsNew(true),
    mNeighbours(new vector<shared_ptr<ShadeGroup>>),
    mForAll(false)
{ }

void ShadeGroupModel::saveChanges()
{
    int pos = mShadeGroup->position();
    mShadeGroup->setChanged(true);
    bool saveAll = mForAll;
    ControllerSchedule schedule;

    int p = 0;    

    if (mForAll) {
        while (p < (int)mNeighbours->size()) {
            mNeighbours->at(p)->setDays(mShadeGroup->days());
            mNeighbours->at(p)->setOpenAt(mShadeGroup->openAt());
            mNeighbours->at(p)->setCloseAt(mShadeGroup->closeAt());
            mNeighbours->at(p)->setChanged(true);
            p++;
        }
    }

    p = 0;
    while (p < (int)mNeighbours->size()) {
        Schedule* s = &schedule.schedules[(int)mNeighbours->at(p)->channel() - 1];
        if (mNeighbours->at(p)->channel() == mShadeGroup->channel()) {
            s->setDays(mShadeGroup->days());
            s->setOpenAt(mShadeGroup->openAt());
            s->setCloseAt(mShadeGroup->closeAt());
        }
        else {
            s->setDays(mNeighbours->at(p)->days());
            s->setOpenAt(mNeighbours->at(p)->openAt());
            s->setCloseAt(mNeighbours->at(p)->closeAt());
        }
        p++;
    }

    if (mOldPosition != pos) {
        auto curr = find_if(mNeighbours->begin(), mNeighbours->end(), [this](const shared_ptr<ShadeGroup>& _g)->bool{ return _g->uuid() == this->mShadeGroup->uuid();});
        if (curr != mNeighbours->end())
            mNeighbours->erase(curr);
        mNeighbours->insert(mNeighbours->begin() + pos, mShadeGroup);
        p = 0;
        while (p < (int)mNeighbours->size()) {
            mNeighbours->at(p)->setPosition(p);
            mNeighbours->at(p)->setChanged(true);
            p++;
        }
        saveAll = true;
    }

    QString controllerUuid = UserData::instance().controllerUuid(mShadeGroup->controllerMac());
    {
        shared_ptr<ControllerAPI> api = ControllerConnectionsManager::instance().controllerAPI(controllerUuid);

        schedule.timezone = UserData::instance().controllerTimezone(controllerUuid);
        QString schedule_json = schedule.json();
        api->updateSchedule(schedule_json);
    }

    if (saveAll)
        UserData::instance().persistChanges(mNeighbours);
    else
        UserData::instance().persistChanged(mShadeGroup);

}

void ShadeGroupModel::deleteGroup()
{
    size_t p = 0;
    bool after_deleted = false;
    while(p < mNeighbours->size()) {
        if (after_deleted) {
            mNeighbours->at(p)->setPosition(p-1);
            mNeighbours->at(p)->setChanged(true);
        }
        else if (mNeighbours->at(p)->uuid() == mShadeGroup->uuid()) {
            mNeighbours->at(p)->setDeleted(true);
            mNeighbours->at(p)->setChanged(true);
            after_deleted = true;
        }
        p++;
    }
    UserData::instance().persistChanges(mNeighbours);
}

void ShadeGroupModel::setUuid(const QString &uuid)
{
    if (uuid == mShadeGroup->uuid()) return;
    shared_ptr<ShadeGroup> group = UserData::instance().shadeGroup(uuid);
    if (group.get()==nullptr) {
        mShadeGroup = shared_ptr<ShadeGroup>(new ShadeGroup);
        mNeighbours->clear();
        mShadeGroup->setPosition(0);
        mOldPosition = 0;
        setNew(true);
    }
    else {
        mShadeGroup = shared_ptr<ShadeGroup>(new ShadeGroup(*group));
        mNeighbours = UserData::instance().shadeGroupsForController(mShadeGroup->controllerMac());
        mOldPosition = mShadeGroup->position();
        setNew(false);
    }
    mForAll = false;
    emit uuidChanged();
    emit nameChanged();
    emit daysChanged();
    emit openAtChanged();
    emit closeAtChanged();
    emit shadeStateChanged();
    emit positionOrderChanged();
    emit positionChanged();
    emit forAllChanged();
}

QString ShadeGroupModel::uuid() const
{
    return mShadeGroup->uuid();
}

QString ShadeGroupModel::controllerMac() const
{
    return mShadeGroup->controllerMac();
}

void ShadeGroupModel::setControllerMac(const QString &mac)
{
    if (mShadeGroup->controllerMac() == mac) return;
    mShadeGroup->setControllerMac(mac);
    mNeighbours = UserData::instance().shadeGroupsForController(mac);
    mShadeGroup->setChannel(findFreeChannel());
    mShadeGroup->setPosition(mNeighbours->size());
    mOldPosition = mShadeGroup->position();
    setNew(true);
    emit controllerMacChanged();
    emit positionOrderChanged();
    emit positionChanged();
}

int ShadeGroupModel::channel() const
{
    return mShadeGroup->channel();
}

void ShadeGroupModel::setChannel(int channel)
{
    if (channel == mShadeGroup->channel()) return;
    mShadeGroup->setChannel(channel);
    emit channelChanged();
}

QString ShadeGroupModel::name() const
{
    return mShadeGroup->name();
}

void ShadeGroupModel::setName(const QString &name)
{
    if (name == mShadeGroup->name()) return;
    mShadeGroup->setName(name);
    emit nameChanged();
}

int ShadeGroupModel::position() const
{
    return mShadeGroup->position();
}

void ShadeGroupModel::setPosition(int position)
{
    if (position == mShadeGroup->position()) return;
    mShadeGroup->setPosition(position);
    emit positionChanged();
}

int ShadeGroupModel::openAt() const
{
    return mShadeGroup->openAt();
}

void ShadeGroupModel::setOpenAt(int openAt)
{
    if (openAt == mShadeGroup->openAt()) return;
    mShadeGroup->setOpenAt(openAt);
    emit openAtChanged();
}

int ShadeGroupModel::closeAt() const
{
    return mShadeGroup->closeAt();
}

void ShadeGroupModel::setCloseAt(int closeAt)
{
    if (closeAt == mShadeGroup->closeAt()) return;
    mShadeGroup->setCloseAt(closeAt);
    emit closeAtChanged();
}

int ShadeGroupModel::days() const
{
    return mShadeGroup->days();
}

void ShadeGroupModel::setDays(int days)
{
    if (days == mShadeGroup->days()) return;
    mShadeGroup->setDays(days);
    emit daysChanged();
}

QString ShadeGroupModel::openAtText() const
{
    return mShadeGroup->openAtText();
}

QString ShadeGroupModel::closeAtText() const
{
    return mShadeGroup->closeAtText();
}

Shade::ShadeState ShadeGroupModel::shadeState() const
{
    return mShadeGroup->state();
}

void ShadeGroupModel::setShadeState(Shade::ShadeState state)
{
    if (state == mShadeGroup->state()) return;
    mShadeGroup->setState(state);
    emit shadeStateChanged();
}

float ShadeGroupModel::openAtUS() const
{
    return mShadeGroup->openAtUS();
}

void ShadeGroupModel::setOpenAtUS(const float openAtUS)
{
    if (openAtUS == mShadeGroup->openAtUS()) return;
    mShadeGroup->setOpenAtUS(openAtUS);
    emit openAtChanged();
}

float ShadeGroupModel::closeAtUS() const
{
    return mShadeGroup->closeAtUS();
}

void ShadeGroupModel::setCloseAtUS(const float closeAtUS)
{
    if (closeAtUS == mShadeGroup->closeAtUS()) return;
    mShadeGroup->setCloseAtUS(closeAtUS);
    emit closeAtChanged();
}

QStringList ShadeGroupModel::getPositionOrder() const
{
    QStringList order;
    order << "first";
    int row = 1;
    int rows = mNeighbours->size();
    int current_row = 0;
    bool found = false;
    while (current_row < rows) {
        if (mNeighbours->at(current_row)->uuid() == mShadeGroup->uuid()) {
            found = true;
            break;
        }
        else current_row++;
    }

    if(found){
        // order for existing
        while(row < rows){
            if(row == rows-1){
                order << "last";
            }
            else if(row < current_row){
                order << QString("before ").append(mNeighbours->at(row)->name());
            }
            else {
                order << QString("before ").append(mNeighbours->at(row + 1)->name());
            }
            row++;
        }
    }
    else{
        // order for created
        while(row < rows){
            order << QString("before ").append(mNeighbours->at(row)->name());
            row++;
        }
        if(rows > 1)
            order << "last";
    }
    return order;
}

char ShadeGroupModel::findFreeChannel() const
{
    char channel = 1;

    auto c = find_if(mNeighbours->begin(), mNeighbours->end(), [&channel](const shared_ptr<ShadeGroup>& _g)->bool{ return _g->channel()==channel; });

    while (c != mNeighbours->end()) {
        channel++;
        c = find_if(mNeighbours->begin(), mNeighbours->end(), [&channel](const shared_ptr<ShadeGroup>& _g)->bool{ return _g->channel()==channel; });
    }
    return channel;
}

bool ShadeGroupModel::isNew() const
{
    return mIsNew;
}

void ShadeGroupModel::setNew(const bool isNew)
{
    mIsNew = isNew;
    emit isNewChanged();
}

void ShadeGroupModel::setForAll(const bool forAll)
{
    if (forAll == mForAll) return;
    mForAll = forAll;
    emit forAllChanged();
}

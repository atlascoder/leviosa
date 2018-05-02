#include "UserData.h"
#include <vector>

template <class T>
void UserData::persistChanges(const shared_ptr<vector<shared_ptr<T>>>& items, bool doReload)
{
    shared_ptr<vector<shared_ptr<Syncable>>> changed(new vector<shared_ptr<Syncable>>);

    for (auto& i : *items) {
        if (i->isChanged()) {
            shared_ptr<Syncable> si(new Syncable(*i));
            si->setValue(i->toJsonString());
            changed->push_back(si);
        }
    }
    if (!changed->empty()) {
        mDb.cognitoSyncDAO.persistChanged(changed);
        if (doReload) {
            loadItems();
            dataChanged();
        }
    }
}

template <class T>
void UserData::persistChanged(const shared_ptr<T>& item, bool doReload)
{
    shared_ptr<vector<shared_ptr<Syncable>>> changed(new vector<shared_ptr<Syncable>>);

    if (item->isChanged()) {
        shared_ptr<Syncable> si(new Syncable(*item));
        si->setValue(item->toJsonString());
        changed->push_back(si);
    }
    if (!changed->empty()) {
        mDb.cognitoSyncDAO.persistChanged(changed);
        if (doReload) {
            loadItems();
            dataChanged();
        }
    }
}

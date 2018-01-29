#ifndef SYNCABLE_H
#define SYNCABLE_H


class Syncable
{
    bool mIsSynced;
public:
    Syncable() : mIsSynced(false) {}
    Syncable(const Syncable& syncable) = default;

    bool isSynced() const { return mIsSynced; }
    void setSynced(bool isSynced) { mIsSynced = isSynced; }
};

#endif // SYNCABLE_H

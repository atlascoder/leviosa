#include "TimeZoneModel.h"

#include <QDateTime>

inline bool isUSZone(const int offset) {
    int offset_hour = offset / 3600;
    return (offset % 3600 == 0) && (offset_hour >= -10) && (offset_hour <= -5);
}


TimeZoneModel::TimeZoneModel(QObject *parent):
    QAbstractListModel(parent)
{
    mCurrentOffset = defaultUtcOffset();
    mSelectedIndex = isUSZone(mCurrentOffset) ? 5 - mCurrentOffset/3600 : 6;
}

int TimeZoneModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return isUSZone(mCurrentOffset) ? 6 : 7;
}

QVariant TimeZoneModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row >= 0 && row < rowCount()) {
        switch (role) {
        case Roles::UtcOffsetRole:
            return row == 6 ? mCurrentOffset : -(5 + row)*3600;
        case Roles::SignatureRole:
            return signatureByIndex(row);
        }
    }
    return QVariant();
}

QHash<int, QByteArray> TimeZoneModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash[Roles::UtcOffsetRole] = "utcOffset";
    hash[Roles::SignatureRole] = "signature";
    return hash;
}

int TimeZoneModel::currentOffset() const
{
    return mCurrentOffset;
}

void TimeZoneModel::setCurrentOffset(const int currentOffset)
{
    if (currentOffset == mCurrentOffset) return;
    mCurrentOffset = currentOffset;
    mSelectedIndex = isUSZone(mCurrentOffset) ? -5 - mCurrentOffset/3600 : 6;
    beginResetModel();
    endResetModel();
    emit selectedChanged();
}

int TimeZoneModel::utcOffset() const
{
    return mSelectedIndex == 6 ? mCurrentOffset : -(mSelectedIndex + 5)*3600;
}

int TimeZoneModel::selectedIndex() const
{
    return mSelectedIndex;
}

void TimeZoneModel::setSelectedIndex(const int index)
{
    if (index == selectedIndex()) return;
    mSelectedIndex = index;
    emit selectedChanged();
}

QString TimeZoneModel::signature() const
{
    return signatureByIndex(mSelectedIndex);
}

QString TimeZoneModel::signatureByIndex(const int index) const
{
    switch (index) {
    case 0:
        return "EST";
    case 1:
        return "CST";
    case 2:
        return "MST";
    case 3:
        return "PST";
    case 4:
        return "AKST";
    case 5:
        return "HAST";
    default:
        if(mCurrentOffset == 0)
            return "UTC";
        else if(mCurrentOffset > 0)
            return "UTC+" + QString::number(mCurrentOffset/3600);
        else
            return "UTC" + QString::number(mCurrentOffset/3600);
    }
}

int TimeZoneModel::defaultUtcOffset()
{
    QDateTime local(QDateTime::currentDateTime());
    QDateTime utc(local.toUTC());
    QDateTime dt(utc.date(), utc.time(), Qt::LocalTime);
    return dt.secsTo(local);
}

QString TimeZoneModel::signatureByOffset(const int utcOffset)
{
    int offset_hours = utcOffset / 3600;
    switch (offset_hours) {
    case -5:
        return "EST";
    case -6:
        return "CST";
    case -7:
        return "MST";
    case -8:
        return "PST";
    case -9:
        return "AKST";
    case -10:
        return "HAST";
    default:
        if(offset_hours == 0)
            return "UTC";
        else if(offset_hours > 0)
            return "UTC+" + QString::number(offset_hours);
        else
            return "UTC" + QString::number(offset_hours);
    }
}

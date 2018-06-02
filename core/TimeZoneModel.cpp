#include "TimeZoneModel.h"

#include <QTimeZone>
#include <QDebug>

static QString US_ZONES[] = {"EST", "CST", "MST", "PST", "AKST", "HAST"};

inline int usZoneIndex(const QString& timezone) {
    int i = 0;
    while (i < 6) {
        if (US_ZONES[i] == timezone)
            return i;
        else
            i++;
    }
    return -1;
}


TimeZoneModel::TimeZoneModel(QObject *parent):
    QAbstractListModel(parent),
    mCurrentTimezone(defaultTimezone())
{
    int idx = usZoneIndex(mCurrentTimezone);
    mSelectedIndex = idx == -1 ? 6 : idx;
}

int TimeZoneModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return usZoneIndex(mCurrentTimezone) == -1 ? 7 : 6;
}

QVariant TimeZoneModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    switch (role) {
    case Roles::NameRole:
        return signatureByIndex(row);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TimeZoneModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash[Roles::NameRole] = "name";
    return hash;
}

QString TimeZoneModel::currentTimezone() const
{
    return mCurrentTimezone;
}

void TimeZoneModel::setCurrentTimezone(const QString &currentTimezone)
{
    if (mCurrentTimezone == currentTimezone) return;
    beginResetModel();
    mCurrentTimezone = currentTimezone;
    int idx = usZoneIndex(mCurrentTimezone);
    mSelectedIndex = idx == -1 ? 6 : idx;
    endResetModel();
    emit currentTimezoneChanged();
    emit selectedChanged();
}

int TimeZoneModel::selectedIndex() const
{
    return mSelectedIndex;
}

void TimeZoneModel::setSelectedIndex(const int index)
{
    if (index == mSelectedIndex) return;
    mSelectedIndex = index;
    emit selectedChanged();
}

QString TimeZoneModel::signatureByIndex(const int index) const
{
    QString result;

    if (index >=0 && index < 6)
        result = US_ZONES[index];
    else
        result = mCurrentTimezone;

    qDebug() << "Timezone [" << index << "] => " << result;
    return result;
}

QString TimeZoneModel::timezone() const
{
    return signatureByIndex(mSelectedIndex);
}

QString TimeZoneModel::defaultTimezone()
{
    QDateTime t = QDateTime::currentDateTime();
    t.setDate(QDate(2000,1,1));
    return t.timeZoneAbbreviation();
}

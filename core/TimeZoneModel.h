#ifndef TIMEZONEMODEL_H
#define TIMEZONEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>

class TimeZoneModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString currentTimezone READ currentTimezone WRITE setCurrentTimezone NOTIFY currentTimezoneChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedChanged)
    Q_PROPERTY(QString timezone READ timezone NOTIFY selectedChanged)
public:
    TimeZoneModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    static QString defaultTimezone();
signals:
    void selectedChanged();
    void currentTimezoneChanged();
private:

    QString mCurrentTimezone;
    int mSelectedIndex;

    QString currentTimezone() const;
    void setCurrentTimezone(const QString& currentOffset);

    int selectedIndex() const;
    void setSelectedIndex(const int index);

    QString timezone() const;

    QString signatureByIndex(const int index) const;
};

#endif // TIMEZONEMODEL_H

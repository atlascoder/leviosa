#ifndef TIMEZONEMODEL_H
#define TIMEZONEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>

class TimeZoneModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int utcOffset READ utcOffset NOTIFY selectedChanged)
    Q_PROPERTY(int currentOffset READ currentOffset WRITE setCurrentOffset NOTIFY selectedChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedChanged)
    Q_PROPERTY(QString signature READ signature NOTIFY selectedChanged)
public:
    TimeZoneModel(QObject *parent = nullptr);

    enum Roles {
        UtcOffsetRole = Qt::UserRole + 1,
        SignatureRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    static int defaultUtcOffset();
    static QString signatureByOffset(const int utcOffset);
signals:
    void selectedChanged();

private:

    int mCurrentOffset;
    int mSelectedIndex;
    bool mUSZone;

    int currentOffset() const;
    void setCurrentOffset(const int currentOffset);

    int selectedIndex() const;
    void setSelectedIndex(const int index);

    QString signature() const;

    int utcOffset() const;

    QString signatureByIndex(const int index) const;
};

#endif // TIMEZONEMODEL_H

#ifndef ALERTBOX_H
#define ALERTBOX_H

#include <QObject>

class AlertBox : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

signals:
    void activeChanged();
    void titleChanged();
    void messageChanged();
    void descriptionChanged();

public slots:
    void hide();

public:
    static AlertBox& instance();

    Q_INVOKABLE void showMessage(const QString& title, const QString& message, const QString& description);

private:
    QString mTitle;
    QString mMessage;
    QString mDescription;
    bool mIsActive;

    AlertBox(QObject *parent=nullptr);

    QString title() const;
    void setTitle(const QString& title);

    QString message() const;
    void setMessage(const QString& message);

    QString description() const;
    void setDescription(const QString& description);

    bool isActive() const;
    void setActive(const bool active);

};

#endif // ALERTBOX_H

#include "AlertBox.h"

AlertBox& AlertBox::instance()
{
    static AlertBox instance;
    return instance;
}

AlertBox::AlertBox(QObject *parent) : QObject(parent)
{

}

QString AlertBox::title() const
{
    return mTitle;
}

void AlertBox::setTitle(const QString &title)
{
    if (title == mTitle) return;
    mTitle = title;
    emit titleChanged();
}

QString AlertBox::message() const
{
    return mMessage;
}

void AlertBox::setMessage(const QString &message)
{
    if (message == mMessage) return;
    mMessage = message;
    emit messageChanged();
}

QString AlertBox::description() const
{
    return mDescription;
}

void AlertBox::setDescription(const QString &description)
{
    if (description == mDescription) return;
    mDescription = description;
    emit descriptionChanged();
}

bool AlertBox::isActive() const
{
    return mIsActive;
}

void AlertBox::setActive(const bool active)
{
    mIsActive = active;
    emit activeChanged();
}

void AlertBox::showMessage(const QString &title, const QString &message, const QString &description)
{
    setTitle(title);
    setMessage(message);
    setDescription(description);
    setActive(true);
}

void AlertBox::hide()
{
    if (mIsActive) {
        mIsActive = false;
        emit activeChanged();
    }
}

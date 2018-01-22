#ifndef SHADE_H
#define SHADE_H

#include <QObject>

class Shade : public QObject
{
    Q_OBJECT
public:
    Shade();

    enum ShadeState { Interim, Opened, Closed, Open, Close, Up, Down, FullOpened, FullClosed };

    Q_ENUMS(ShadeState)

    ShadeState state() const;
    void setState(ShadeState state);

private:
    ShadeState mState;
};

#endif // SHADE_H

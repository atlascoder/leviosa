#ifndef POSITIONED_H
#define POSITIONED_H

#include <QString>
#include <memory>

class Positioned
{
protected:
    int mPosition;
public:
    Positioned();

    int position() const;
    void setPosition(int position);

};

#endif // POSITIONED_H
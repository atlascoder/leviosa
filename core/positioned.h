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
    Positioned(const Positioned& positioned) = default;

    int position() const;
    void setPosition(int position);

};

#endif // POSITIONED_H

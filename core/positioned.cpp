#include "positioned.h"
#include <QStringList>
Positioned::Positioned():
    mPosition(0)
{

}

int Positioned::position() const
{
    return mPosition;
}

void Positioned::setPosition(int position)
{
    mPosition = position;
}

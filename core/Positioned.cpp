#include "Positioned.h"
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

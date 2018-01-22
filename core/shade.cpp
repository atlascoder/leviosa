#include "shade.h"

Shade::Shade(): QObject(), mState(ShadeState::Interim)
{

}

Shade::ShadeState Shade::state() const
{
    return mState;
}

void Shade::setState(ShadeState state){
    mState = state;
}

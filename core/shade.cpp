#include "shade.h"

Shade::Shade(int id):
    mId(id), mState(State::Undefined)
{

}

int Shade::id() const
{
    return mId;
}

void Shade::setId(int id)
{
    mId = id;
}

Shade::State Shade::getState() const
{
    return mState;
}

void Shade::setState(State state){
    mState = state;
}

void Shade::open()
{
    if(mState != State::Opened){
        // do open
    }
}

void Shade::close()
{
    if(mState != State::Closed){
        // do close
    }
}

void Shade::stop()
{
    if(mState == State::Opening || mState == State::Closing){
        // do stop
    }
}

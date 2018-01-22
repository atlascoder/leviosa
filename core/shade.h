#ifndef SHADE_H
#define SHADE_H

class Shade
{
public:
    explicit Shade(int id);

    enum class State { Undefined, Middle, Opened, Closed, Opening, Closing };

    int id() const;
    void setId(int id);

    State getState() const;
    void setState(State state);

    void open();
    void close();
    void stop();

private:
    int mId;
    State mState;

};

#endif // SHADE_H

#include "Actor.h"
#include "StudentWorld.h"
#include <memory>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
void Iceman::doSomething() {
    int val;
    // reading user input
    if (!getWorld()->getKey(val)) {
        return;
    }
    // player moving mechanics
    std::shared_ptr<Actor> ptr;
    switch (val) {
            // move left
        case KEY_PRESS_LEFT:
            // Check the position of iceman that it is in bound
            getWorld()->getField(getX()-1, getY(), ptr);
            if (getX() <= 0 || ptr != NULL) {
                break;
            }
            // Move the iceman
            getWorld()->setField(getX(), getY(), nullptr);
            setDirection(left);
            moveTo(getX()-1, getY());
            getWorld()->setField(getX(), getY(), getWorld()->getIceman());
            break;
            // move right
        case KEY_PRESS_RIGHT:
            getWorld()->getField(getX()+1, getY(), ptr);
            if (getX() >= 63 || ptr != NULL) {
                break;
            }
            getWorld()->setField(getX(), getY(), nullptr);
            setDirection(right);
            moveTo(getX()+1, getY());
            getWorld()->setField(getX(), getY(), getWorld()->getIceman());
            break;
            // move upwards
        case KEY_PRESS_UP:
            getWorld()->getField(getX(), getY()+1, ptr);
            if (getY() >= 63 || ptr != NULL) {
                break;
            }
            getWorld()->setField(getX(), getY(), nullptr);
            setDirection(up);
            moveTo(getX(), getY()+1);
            getWorld()->setField(getX(), getY(), getWorld()->getIceman());
            break;
            // move downwards
        case KEY_PRESS_DOWN:
            getWorld()->getField(getX(), getY()-1, ptr);
            if (getY() <= 0 || ptr != NULL) {
                break;
            }
            getWorld()->setField(getX(), getY(), nullptr);
            setDirection(down);
            moveTo(getX(), getY()-1);
            getWorld()->setField(getX(), getY(), getWorld()->getIceman());
            
            break;
        default:
            break;
    }
    breakIce();
}
// Ice breaking mechanics
void Iceman::breakIce() {
    bool dig = false;
    for (int x = getX(); x <= getX()+3; x++) {
        for (int y = getY(); y <= getY()+3; y++) {
            if (getWorld()->breakIce(x, y)) {
                dig = true;
            }
        }
    }
    if (dig) {
        getWorld()->playSound(SOUND_DIG);
    }
}


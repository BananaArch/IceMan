#include "Actor.h"
#include "StudentWorld.h"
#include <memory>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
void Iceman::doSomething() {
    int val;
    if (!getWorld()->getKey(val)) {
        return;
    }
    std::shared_ptr<Actor> ptr;
    switch (val) {
        // move left
        case KEY_PRESS_LEFT:
            getWorld()->getField(getX()-1, getY(), ptr);
            if (getX() <= 0 || ptr != NULL) {
                break;
            }
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
}

void RegularProtestor::doSomething() {
    // TODO: Add actual logic
}

void HardcoreProtestor::doSomething() {
    // TODO: Add actual logic
}

void Boulder::doSomething() {
    // TODO: Add actual logic
}

void Spurt::doSomething() {
    // TODO: Add actual logic
}

void Pool::doSomething() {
    // TODO: Add actual logic
}

void OilBarrel::doSomething() {
    // TODO: Add actual logic
}

void Sonar::doSomething() {
    // TODO: Add actual logic
}

void Gold::doSomething() {
    // TODO: Add actual logic
}
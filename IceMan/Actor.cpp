#include "Actor.h"
#include "StudentWorld.h"
#include <memory>
using namespace std;
// Iceman move and dig mechanics
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
            setDirection(left);
            moveTo(getX()-1, getY());
            break;
            // move right
        case KEY_PRESS_RIGHT:
            getWorld()->getField(getX()+1, getY(), ptr);
            if (getX() >= 63 || ptr != NULL) {
                break;
            }
            setDirection(right);
            moveTo(getX()+1, getY());
            break;
            // move upwards
        case KEY_PRESS_UP:
            getWorld()->getField(getX(), getY()+1, ptr);
            if (getY() >= 63 || ptr != NULL) {
                break;
            }
            setDirection(up);
            moveTo(getX(), getY()+1);
            break;
            // move downwards
        case KEY_PRESS_DOWN:
            getWorld()->getField(getX(), getY()-1, ptr);
            if (getY() <= 0 || ptr != NULL) {
                break;
            }
            setDirection(down);
            moveTo(getX(), getY()-1);
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

void Boulder::doSomething() {
    if (!stable) {
        if (fallTimer < 30) {
            fallTimer++;
        }
        else {
            int x = getX(), y = getY();
            bool crash = false;
            // check if the bolder hits the bottom
            if (y < 0)
                crash = true;
            // check if the bolder hits ice
            for (int i = x; i < x+4; i++) {
                if (getWorld()->getIce(i, y-1) != nullptr) {
                    crash = true;
                }
            }
            // check if the bolder hits bolder
            for (auto b : getWorld()->getBoulders()) {
                if ((b->getX() <= x && b->getX() + 3 >= x) && b->getY() == this->getY()-4) {
                        crash = true;
                }
            }
            // check if the bolder hits player
            if ((getWorld()->getIceman()->getX() <= x && getWorld()->getIceman()->getX() + 3 >= x) && getWorld()->getIceman()->getY() == this->getY()-4) {
                getWorld()->getIceman()->annoyed(10);
            }
            // Keep falling
            if (!crash) {
                moveTo(x, y-1);
                getWorld()->playSound(SOUND_FALLING_ROCK);
            }
            // The boulder crashes and gets destroyed
            else {
                unalive();
                setVisible(false);
            }
        }
    }
    else {
        // check if ice below are still there
        unsigned count = 0;
        int y = getY() - 1;
        for (int x = getX(); x < getX()+4; x++) {
            if (getWorld()->getIce(x, y) == nullptr)
                count++;
        }
        if (count == 4) {
            stable = false;
            getWorld()->setField(getX(), getY(), nullptr);
        }
    }
}

void OilBarrel::doSomething() {
    int dist_x, dist_y;
    dist_x = getX() - getWorld()->getIceman()->getX();
    dist_y = getY() - getWorld()->getIceman()->getY();
    if (!isVisible()) {
        if ((dist_x * dist_x + dist_y * dist_y) <= 16) {
            setVisible(true);
        }
    }
    else {
        if ((dist_x * dist_x + dist_y * dist_y) <= 9) {
            setVisible(false);
            unalive();
            getWorld()->playSound(SOUND_FOUND_OIL);
        }
    }
}

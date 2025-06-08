#include "Actor.h"
#include "StudentWorld.h"
#include <memory>
#include <queue>
using namespace std;

// TODO
void Person::move(MoveDirection dir) {
    switch (dir) {
    case moveUp:
        // check that it's in bounds and does not contain boulder
        if (getY() < 60 && !getWorld()->containsBoulder(getX(), getY() + 1))
            moveTo(getX(), getY() + 1);
        break;
    case moveDown:
        // check that it's in bounds and does not contain boulder
        if (getY() > 0 && !getWorld()->containsBoulder(getX(), getY() - 1))
            moveTo(getX(), getY() - 1);
        break;
    case moveRight:
        // check that it's in bounds and does not contain boulder
        if (getX() < 60 && !getWorld()->containsBoulder(getX() + 1, getY()))
            moveTo(getX() + 1, getY());
        break;
    case moveLeft:
        // check that it's in bounds and does not contain boulder
        if (getX() > 0 && !getWorld()->containsBoulder(getX() - 1, getY()))
            moveTo(getX() - 1, getY());
        break;
    default:
        break;
    }
}

// Iceman move and dig mechanics
void Iceman::doSomething() {

    if (getHp() <= 0 || !isAlive()) {
        unalive();
        return;
    }

    int val;
    // reading user input
    if (!getWorld()->getKey(val)) {
        return;
    }
    // player moving mechanics
    std::shared_ptr<Actor> ptr;
    switch (val) {
            // unalive themselves
        case KEY_PRESS_ESCAPE:
            setHp(0);
            break;
            // TODO: implement water squirt logic
        case KEY_PRESS_SPACE: // page 28 part b
            break;
        case 'z': // TODO: implement sonar logic
        case 'Z':
            break;
        case KEY_PRESS_TAB: // TODO: Implement gold logic
            break;
        case KEY_PRESS_LEFT:
            if (getDirection() == left) {
                move(moveLeft);
            }
            else {
                setDirection(left);
            }
            break;
            // move right
        case KEY_PRESS_RIGHT:
            if (getDirection() == right) {
                move(moveRight);
            }
            else {
                setDirection(right);
            }
            break;
            // move upwards
        case KEY_PRESS_UP:
            if (getDirection() == up) {
                move(moveUp);
            }
            else {
                setDirection(up);
            }
            break;
            // move downwards
        case KEY_PRESS_DOWN:
            if (getDirection() == down) {
                move(moveDown);
            }
            else {
                setDirection(down);
            }
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

void Iceman::decreaseHp(int dmg)  {
    Person::decreaseHp(dmg);
    if (getHp() > 0)
        getWorld()->playSound(SOUND_PLAYER_ANNOYED);
}

// constructor for protester
Protester::Protester(int imageId, int x, int y, StudentWorld* world, int hp) : Person(imageId, x, y, left, 1.0, 0, world, hp), m_isLeavingField(false) {
    m_numSquaresToMoveInCurrentDirections = 8 + (std::rand() % (60 - 8 + 1));
    m_ticksToWaitBetweenMoves = (0 > (3 - getWorld()->getLevel() / 4)) ? 0 : (3 - getWorld()->getLevel() / 4); // max using ternary operator
    setVisible(true);
}

int Protester::isARestTick() {
    return getWorld()->getTickCount() % m_ticksToWaitBetweenMoves != 0;
}

void Protester::moveToLeaveLocation() {
    move(getWorld()->bestDirectionToReturn(getX(), getY()));
}

void Protester::moveToPlayer() {
    move(getWorld()->bestDirectionToPlayer(getX(), getY()));
}

void RegularProtester::doSomething() {
    if (!isAlive()) return;
    
    // checks if it is a rest tick
    if (isARestTick()) {
        std::cout << "rest tick" << std::endl;
        return;
    }

    std::cout << "not a rest tick" << std::endl;

    // check if hp is zero or less, if so then it leaves field
    if (!getIsLeaving() && getHp() <= 0) {
        setIsLeaving(true);
    }

    // if it's in the leaving field state
    if (getIsLeaving()) {
        // if it's at the exit point
        if (getX() == 60 && getY() == 60) {
            unalive();
            return;
        }
        moveToLeaveLocation();
        return;
    }

    moveToPlayer();

}

void HardcoreProtester::doSomething() {}

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
                if (b->getX() <= x && b->getX() + 3 >= x && b->getY() == this->getY()-4) {
                        crash = true;
                }
            }
            // check if the boulder hits player
            if (getWorld()->getIceman()->getX() <= x && getWorld()->getIceman()->getX() + 3 >= x && getWorld()->getIceman()->getY() == this->getY()-4) {
                getWorld()->getIceman()->setHp(0);
            }
            
            // check if the boulder hits protestors
            std::vector<std::shared_ptr<Protester>> pList;
            getWorld()->getpList(pList);
            for (auto it = pList.begin(); it != pList.end(); ++it) {
                // check if it's null just in case
                if (!(*it)) continue;
                
                int px = (*it)->getX();
                int py = (*it)->getY();
                // if a protestor is hit, damage it and increase score
                if (px <= x && px + 3 >= x && py == this->getY() - 4) {
                    (*it)->decreaseHp(20);
                    getWorld()->increaseScore(500);
                }
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
            getWorld()->playSound(SOUND_FOUND_OIL);
            getWorld()->increaseScore(1000);
            unalive();
        }
    }
}

void Gold::doSomething() {
    // if golden nugget is not picked up by player yet
    if (!picked) {
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
                getWorld()->playSound(SOUND_GOT_GOODIE);
                getWorld()->increaseScore(10);
                getWorld()->getIceman()->pickGold();
                unalive();
            }
        }
    }
    // if golden nugget is picked and then placed by player
    else {
        std::vector<std::shared_ptr<Protester>> pList;
        getWorld()->getpList(pList);
        for (auto it = pList.begin(); it != pList.end(); ++it) {
            // check if it's null just in case
            if (!(*it)) continue;
            
            int x = (*it)->getX();
            int y = (*it)->getY();
            // if a protestor is hit, damage it and increase score
            int dist_x = getX() - x, dist_y = getY() - y;
            // when protestor is close enough, pick up gold and leave
            if ((dist_x * dist_x + dist_y * dist_y) <= 9) {
                setVisible(false);
                getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
                getWorld()->increaseScore(25);
                unalive();
                (*it)->setIsLeaving(true);
                break;
            }
        }
    }
}

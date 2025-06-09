#include "Actor.h"
#include "StudentWorld.h"
#include <memory>
#include <queue>
#include <cmath>
using namespace std;

void Person::move(MoveDirection dir) {
    switch (dir) {
    case moveUp:
        // check that it's in bounds and does not contain boulder
        if (getY() < 60 && !getWorld()->containsBoulder(getX(), getY() + 1)) {
            moveTo(getX(), getY() + 1);
            setDirection(up);
        }
        break;
    case moveDown:
        // check that it's in bounds and does not contain boulder
        if (getY() > 0 && !getWorld()->containsBoulder(getX(), getY() - 1)) {
            moveTo(getX(), getY() - 1);
            setDirection(down);
        }
        break;
    case moveRight:
        // check that it's in bounds and does not contain boulder
        if (getX() < 60 && !getWorld()->containsBoulder(getX() + 1, getY())) {
            moveTo(getX() + 1, getY());
            setDirection(right);
        }
        break;
    case moveLeft:
        // check that it's in bounds and does not contain boulder
        if (getX() > 0 && !getWorld()->containsBoulder(getX() - 1, getY())) {
            moveTo(getX() - 1, getY());
            setDirection(left);
        }
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
    getWorld()->playSound(SOUND_PLAYER_ANNOYED);
}

// constructor for protester
Protester::Protester(int imageId, int x, int y, StudentWorld* world, int hp) : Person(imageId, x, y, left, 1.0, 0, world, hp), m_isLeavingField(false), m_nonRestingTickSinceAnnoyingPlayer(0), m_nonRestingTicksSincePerpendicularMove(0), m_currentDirection(moveNone), m_stunTicks(0) {
    m_numSquaresToMoveInCurrentDirections = 8 + (std::rand() % (60 - 8 + 1));
    m_ticksToWaitBetweenMoves = (0 > (3 - getWorld()->getLevel() / 4)) ? 0 : (3 - getWorld()->getLevel() / 4); // max using ternary operator
    setVisible(true);
}

int Protester::isARestTick() {
    return (m_ticksToWaitBetweenMoves == 0) ? false : getWorld()->getTickCount() % m_ticksToWaitBetweenMoves != 0;
}

void Protester::moveToLeaveLocation() {
    move(getWorld()->bestDirectionToReturn(getX(), getY()));
}

void Protester::moveToPlayer() {
    move(getWorld()->bestDirectionToPlayer(getX(), getY()));
}

bool Protester::canMoveTo(int x, int y) {
    return x >= 0 && x < 64 && y >= 0 && y < 64 &&
        !getWorld()->containsIce(x, y) &&
        !getWorld()->containsBoulder(x, y);
}

bool Protester::canMoveInDirection(MoveDirection dir) {
    int x = getX();
    int y = getY();
    switch (dir) {
    case moveUp:    return canMoveTo(x, y + 1);
    case moveDown:  return canMoveTo(x, y - 1);
    case moveLeft:  return canMoveTo(x - 1, y);
    case moveRight: return canMoveTo(x + 1, y);
    default:        return false;
    }
}

void Protester::pickRandomValidDirection() {
    bool foundDirection = false;

    while (!foundDirection) {
        int dir = rand() % 4;
        MoveDirection tryDirection;

        switch (dir) {
        case 0: tryDirection = moveRight; break;
        case 1: tryDirection = moveLeft;  break;
        case 2: tryDirection = moveDown;  break;
        case 3: tryDirection = moveUp;    break;
        default: continue; // Shouldn't happen, but skip to next iteration just in case
        }

        if (canMoveInDirection(tryDirection)) {
            setCurrentDirection(tryDirection);
            foundDirection = true;
        }
    }
}


void RegularProtester::doSomething() {
    if (!isAlive()) return;

    if (getStunTicks() >= 0) {
        decrementStunTicks();
        return;
    }
    
    // checks if it is a rest tick
    if (isARestTick()) {
        return;
    }

    // increment nonRestingTicksSinceLastAnnoyingPlayer since this is not a resting tick.
    // if the protester DOES annoy a player this tick, then we reset it later

    incrementNonRestingTicksSinceAnnoyingPlayer();

    // increment nngTicksSincePerpendicularMove since this is not a resting tick.
    // if the protester DOES move perpendicularly during this tick, then we reset it later

    incrementNonRestingTicksSincePerpendicularMove();

    // check if hp is zero or less, if so then it leaves field
    if (!getIsLeaving() && getHp() <= 0) {
        setIsLeaving(true);
        setTicksToWaitBetweenMoves(0);
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

    int icemanX = getWorld()->getIceman()->getX();
    int icemanY = getWorld()->getIceman()->getY();
    int protesterX = getX();
    int protesterY = getY();

    int dx = protesterX - icemanX;
    int dy = protesterY - icemanY;
    double distanceToIceman = sqrt(dx * dx + dy * dy);

    // Annoy Mechanism -- STEP FOUR
    
    // if protester hasn't shoulted within its last non-resting 15 ticks
    // AND they are within distance of 4 units of Iceman
    // AND is facing iceman

    if (getNonRestingTicksSinceAnnoyingPlayer() > 15 && distanceToIceman <= 4.0 && isFacingIceman(icemanX, icemanY)) {
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->getIceman()->decreaseHp(2);
        resetNonRestingTicksSinceAnnoyingPlayer();
        return;
    }

    // Walking to player mechanism (if direct line of sight) -- STEP FIVE

    // If Iceman is in a straight horizontal or vertical line of sight to the protester
    // (even if the protester isn't facing Iceman)
    // AND if the protester is more than 4 units away from Iceman
    if ((protesterX == icemanX || protesterY == icemanY) && distanceToIceman > 4) {

        // Booleans to track whether a clear path exists to Iceman along X or Y axis
        bool walkableToIcemanInX = true;
        bool walkableToIcemanInY = true;

        // Case 1: Iceman and protester are in the same row 
        if (protesterY == icemanY) {
            // Determine direction to iterate: +1 if Iceman is to the right, -1 if to the left
            int signumX = (icemanX > protesterX) - (icemanX < protesterX);

            // Check each tile between protester and Iceman for obstructions
            for (int i = protesterX + signumX; i != icemanX; i += signumX) {
                // If a boulder or ice is in the way, path is not walkable
                if (getWorld()->containsBoulder(i, protesterY) || getWorld()->containsIce(i, protesterY)) {
                    walkableToIcemanInX = false;
                    break;
                }
            }

            // If path is clear, move toward Iceman along X direction 
            if (walkableToIcemanInX) {
                moveToPlayer(); // move in X direction
                setNumSquaresToMoveInCurrentDirection(0);
                return;
            }
        }

        // Case 2: Iceman and protester are in the same column 
        if (protesterX == icemanX) {
            // Determine direction to iterate: +1 if Iceman is below, -1 if above
            int signumY = (icemanY > protesterY) - (icemanY < protesterY);

            // Check each tile between protester and Iceman for obstructions
            for (int j = protesterY + signumY; j != icemanY; j += signumY) {
                // If a boulder or ice is in the way, path is not walkable
                if (getWorld()->containsBoulder(protesterX, j) || getWorld()->containsIce(protesterX, j)) {
                    walkableToIcemanInY = false;
                    break;
                }
            }

            // If path is clear, move toward Iceman along Y direction 
            if (walkableToIcemanInY) {
                moveToPlayer(); // move in Y direction
                setNumSquaresToMoveInCurrentDirection(0);
                return;
            }
        }
    }
    else {

        // IF PROTESTER CANT DIRECTLY SEE ICEMAN (STEP SIX)

        decrementNumSquaresToMoveInCurrentDirection();

        if (getNumSquaresToMoveInCurrentDirection() <= 0) {
            // choose a new random direction
            pickRandomValidDirection();
            // choose new random stepcount from [8,60]
            int steps = 8 + rand() % (60 - 8 + 1);
            setNumSquaresToMoveInCurrentDirection(steps);
        }
    }

    // The Regular Protester hasn’t made a perpendicular turn in the last 200 nonresting ticks
    if (getNonRestingTicksSincePerpendicularMove() > 200) {
        
        MoveDirection currentDirection = getCurrentDirection();
        bool facingHorizontally = (currentDirection == moveLeft || currentDirection == moveRight);
        bool facingVertically = (currentDirection == moveUp || currentDirection == moveDown);
        std::vector<MoveDirection> perpendicularOptions;

        if (facingHorizontally) {
            if (canMoveInDirection(moveUp)) perpendicularOptions.push_back(moveUp);
            if (canMoveInDirection(moveDown)) perpendicularOptions.push_back(moveDown);
        } else if (facingVertically) {
            if (canMoveInDirection(moveLeft)) perpendicularOptions.push_back(moveLeft);
            if (canMoveInDirection(moveRight)) perpendicularOptions.push_back(moveRight);
        }

        if (!perpendicularOptions.empty()) {
            // Choose randomly among available perpendicular directions
            int randomIndex = rand() % perpendicularOptions.size();
            MoveDirection currentDirection = perpendicularOptions[randomIndex];

            // Set new direction 
            setCurrentDirection(currentDirection);

            int newSteps = 8 + rand() % (60 - 8 + 1);
            setNumSquaresToMoveInCurrentDirection(newSteps);

            resetNonRestingTicksSincePerpendicularMove();
        }
    }

    if (canMoveInDirection(getCurrentDirection())) move(getCurrentDirection());
    else setNumSquaresToMoveInCurrentDirection(0);

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
                if ((b->getX() <= x && b->getX() + 3 >= x) && b->getY() == this->getY()-4) {
                        crash = true;
                }
            }
            // check if the boulder hits player
            if ((getWorld()->getIceman()->getX() <= x && getWorld()->getIceman()->getX() + 3 >= x) && getWorld()->getIceman()->getY() == this->getY()-4) {
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

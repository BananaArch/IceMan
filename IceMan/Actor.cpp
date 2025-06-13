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
        case KEY_PRESS_SPACE: 
            if (m_water > 0) {
                getWorld()->createSquirt(this);
                useWater();
            }
                
            break;
        case 'z': 
            if (getWorld()->getIceman()->getSonar() > 0) {
                getWorld()->getIceman()->useSonar();
                getWorld()->sonarScan();
            }
            break;
        case 'Z':
            if (getWorld()->getIceman()->getSonar() > 0) {
                getWorld()->getIceman()->useSonar();
                getWorld()->sonarScan();
            }
            break;
        case KEY_PRESS_TAB: // drop gold
            if (getGold() == 0)
                return;
            getWorld()->addGold(getX(), getY());
            dropGold();
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
Protester::Protester(int imageId, int x, int y, StudentWorld* world, int hp) : Person(imageId, x, y, left, 1.0, 0, world, hp), m_isLeavingField(false), m_nonRestingTickSinceAnnoyingPlayer(0), m_nonRestingTicksSincePerpendicularMove(0), m_currentDirection(moveNone), m_stunTicks(0) {
    m_numSquaresToMoveInCurrentDirections = 8 + (std::rand() % (60 - 8 + 1));
    m_ticksToWaitBetweenMoves = (0 > (3 - getWorld()->getLevel() / 4)) ? 0 : (3 - getWorld()->getLevel() / 4); // max using ternary operator
    m_maxStun = (100 - getWorld()->getLevel() * 10 > 50 ? 100 - getWorld()->getLevel() * 10 : 50);
    setVisible(true);
}

void Protester::decreaseHp(int dmg) {
    Person::decreaseHp(dmg);
    if (getHp() > 0) {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    }
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
    return x >= 0 && x <= 61 && y >= 0 && y <= 61 &&
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

bool Protester::isFacingIceman(int icemanX, int icemanY) const {
    Direction direction = getDirection();
    switch (direction) {
    case right:
        return icemanX > getX() && abs(icemanY - getY()) < 4; // if iceman is to the right of the protester AND they're on same height
    case left:
        return icemanX < getX() && abs(icemanY - getY()) < 4; // if iceman is to the left of the protester AND they're on same height
    case up:
        return icemanY > getY() && abs(icemanX - getX()) < 4; // if iceman is to the above of the protester AND they're on same column
    case down:
        return icemanY < getY() && abs(icemanX - getX()) < 4; // if iceman is to the below of the protester AND they're on same column
    default:
        return false;
    }
}

double Protester::getEuclidianDistanceTo(int x, int y) const {
    int protesterX = getX();
    int protesterY = getY();

    int dx = protesterX - x;
    int dy = protesterY - y;
    return sqrt(dx * dx + dy * dy);
}

// helper function for doSomething shared by both regular and hardcore protester
// returns whether or not it should exit main doSomething() function
// handles preaction things such as, checking ifalive, dealing with rest and stun ticks, moving to leave location if leaving
bool Protester::doSomethingPreAction() {

    if (!isAlive()) return true;

    if (getStunTicks() >= 0) {
        decrementStunTicks();
        return true;
    }

    // checks if it is a rest tick
    if (isARestTick()) {
        return true; // exit if it is
    }

    // increment nonRestingTicksSinceLastAnnoyingPlayer since this is not a resting tick.
    // if the protester DOES annoy a player this tick, then we reset it later

    incrementNonRestingTicksSinceAnnoyingPlayer();

    // increment nonRestingTicksSincePerpendicularMove since this is not a resting tick.
    // if the protester DOES move perpendicularly during this tick, then we reset it later

    incrementNonRestingTicksSincePerpendicularMove();

    // check if hp is zero or less, if so then it leaves field
    if (!getIsLeaving() && getHp() <= 0) {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        setIsLeaving(true);
        setTicksToWaitBetweenMoves(0);
    }

    // if it's in the leaving field state
    if (getIsLeaving()) {
        // if it's at the exit point
        if (getX() == 60 && getY() == 60) {
            unalive();
            return true;
        }
        moveToLeaveLocation();
        return true;
    }
    return false;
}

// helper function for doSomething shared by both regular and hardcore protester
// returns whether or not it should exit main doSomething() function
// handles trying to damage the player if it gets within 4 units, and is facing iceman, and hasn't annoyed iceman in past 15 nonresting ticks
bool Protester::tryToAnnoyPlayer() {
    
    int icemanX = getWorld()->getIceman()->getX();
    int icemanY = getWorld()->getIceman()->getY();
    
    double distanceToIceman = getEuclidianDistanceTo(icemanX, icemanY);

    // if protester hasn't shoulted within its last non-resting 15 ticks
    // AND they are within distance of 4 units of Iceman
    // AND is facing iceman
    if (getNonRestingTicksSinceAnnoyingPlayer() > 15 && distanceToIceman <= 4.0 && isFacingIceman(icemanX, icemanY)) {
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->getIceman()->decreaseHp(2);
        resetNonRestingTicksSinceAnnoyingPlayer();
        return true;
    }

    return false;
}

// helper function for doSomething shared by both regular and hardcore protester
// returns whether or not it should exit main doSomething() function
// handles trying to walk to player if protester is in direct line of sight
bool Protester::tryDirectLineMoveToPlayer() {
    
    int icemanX = getWorld()->getIceman()->getX();
    int icemanY = getWorld()->getIceman()->getY();
    int protesterX = getX();
    int protesterY = getY();

    double euclidianDistanceToIceman = getEuclidianDistanceTo(icemanX, icemanY);

    // If Iceman is in a straight horizontal or vertical line of sight to the protester
    // (even if the protester isn't facing Iceman)
    // AND if the protester is more than 4 units away from Iceman

    if ((protesterX == icemanX || protesterY == icemanY) && euclidianDistanceToIceman > 4.0) {

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
                return true;
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
                return true;
            }
        }
    }

    return false;
}

// helper function for doSomething shared by both regular and hardcore protester
// handles logic for randomly moving and randomly switching directions
// also handles logic for intersections
void Protester::randomlyMove() {
    decrementNumSquaresToMoveInCurrentDirection();

    if (getNumSquaresToMoveInCurrentDirection() <= 0) {
        // choose a new random direction
        pickRandomValidDirection();
        // choose new random stepcount from [8,60]
        int steps = 8 + rand() % (60 - 8 + 1);
        setNumSquaresToMoveInCurrentDirection(steps);
    }

    // The Regular Protester hasn�t made a perpendicular turn in the last 200 nonresting ticks
    if (getNonRestingTicksSincePerpendicularMove() > 200) {

        MoveDirection currentDirection = getCurrentDirection();
        bool facingHorizontally = (currentDirection == moveLeft || currentDirection == moveRight);
        bool facingVertically = (currentDirection == moveUp || currentDirection == moveDown);
        std::vector<MoveDirection> perpendicularOptions;

        if (facingHorizontally) {
            if (canMoveInDirection(moveUp)) perpendicularOptions.push_back(moveUp);
            if (canMoveInDirection(moveDown)) perpendicularOptions.push_back(moveDown);
        }
        else if (facingVertically) {
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

// helper function for doSomething for the hardcore protester
// returns whether or not it should exit main doSomething() function
// handles tracking cellphone and using BFS algorithm to track and move through shortest path to iceman
bool HardcoreProtester::tryToTrackPhone() {

    int icemanX = getWorld()->getIceman()->getX();
    int icemanY = getWorld()->getIceman()->getY();

    double euclidianDistanceToIceman = getEuclidianDistanceTo(icemanX, icemanY);

    if (euclidianDistanceToIceman > 4.0) {
        // this is different then euclidian distance, since it uses pathfinding to find shortest distance
        int shortestDistanceFromPlayer = getWorld()->getShortestDistanceToPlayer(getX(), getY());
        if (shortestDistanceFromPlayer < m_M) {
            moveToPlayer();
            return true;
        }
    }
    
    return false;
}

void RegularProtester::doSomething() {

    if (doSomethingPreAction()) return;
    if (tryToAnnoyPlayer()) return;
    if (tryDirectLineMoveToPlayer()) return;
    randomlyMove();

}

void HardcoreProtester::doSomething() {
    
    if (doSomethingPreAction()) return;
    if (tryToAnnoyPlayer()) return;
    if (tryToTrackPhone()) return;
    if (tryDirectLineMoveToPlayer()) return;
    randomlyMove();

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
        // Dropped gold can only survive for 100 ticks
        if (getTimer() >= getMaxLife()) {
            setVisible(false);
            unalive();
        }
        // increase life timer
        incTimer();
        // check if it's picked up by a protester
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

void Pool::doSomething() {
    // unalive when reaching max life-time
    if (getTimer() >= getMaxLife()) {
        unalive();
        return;
    }
    // increase life timer
    incTimer();
    // look for iceman position
    int x = getWorld()->getIceman()->getX();
    int y = getWorld()->getIceman()->getY();
    int dist_x = getX() - x, dist_y = getY() - y;
    // when player is close enough, pick up water pool
    if ((dist_x * dist_x + dist_y * dist_y) <= 9) {
        setVisible(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(100);
        getWorld()->getIceman()->pickPool();
        unalive();
    }
}

void Sonar::doSomething() {
    // unalive when reaching max life-time
    if (getTimer() >= getMaxLife()) {
        unalive();
        return;
    }
    // increase life timer
    incTimer();
    // look for iceman position
    int x = getWorld()->getIceman()->getX();
    int y = getWorld()->getIceman()->getY();

    int dist_x = getX() - x, dist_y = getY() - y;
    // when player is close enough, pick up water pool
    if ((dist_x * dist_x + dist_y * dist_y) <= 9) {
        setVisible(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(75);
        getWorld()->getIceman()->pickSonar();
        unalive();
    }
}

bool Squirt::canMoveTo(int x, int y) {
    return x >= 0 && x < 61 && y >= 0 && y < 61 &&
        !getWorld()->containsIce(x, y) &&
        !getWorld()->containsBoulder(x, y);
}

void Squirt::doSomething() {

    if (!isAlive()) return;

    // if there is a protester here, it will damage it and destroy itself and return
    if (getWorld()->damageProtesters(getX(), getY())) {
        unalive();
        return;
    }
    
    // if it has already travelled 4 units, it will destroy itself and return
    if (m_distanceTravelled >= 4) {
        unalive();
        return;
    }

    int x = getX();
    int y = getY();

    switch (getDirection()) {
    case up:
        if (canMoveTo(x, y + 1)) { // if it can move
            moveTo(x, y + 1); // it will move
            m_distanceTravelled++;
            return;
        }
        else { // otherwise it will destroy itself
            unalive();
            return;
        }
    case down:
        if (canMoveTo(x, y - 1)) {
            moveTo(x, y - 1);
            m_distanceTravelled++;
            return;
        }
        else {
            unalive();
            return;
        }
    case left:
        if (canMoveTo(x - 1, y)) {
            moveTo(x - 1, y);
            m_distanceTravelled++;
            return;
        }
        else {
            unalive();
            return;
        }
    case right:
        if (canMoveTo(x + 1, y)) { 
            moveTo(x + 1, y);
            m_distanceTravelled++;
            return;
        }
        else {
            unalive();
            return;
        }
    default: // in case something unexpected happens, it kills itself
        unalive();
        return;
    }
}
#ifndef ACTOR_H_
#define ACTOR_H_
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#include "GraphObject.h"
#include "GameConstants.h"
#include "SpriteManager.h"

#include<cstdlib>
enum MoveDirection { moveNone, moveUp, moveRight, moveLeft, moveDown };

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
// Base class of all actors
class Actor : public GraphObject {
public:
    Actor(int imageID, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world) : GraphObject(imageID, x, y, dir, size, depth), m_alive(true), m_world(world) {}
    virtual ~Actor() {}
    virtual void doSomething() = 0; // pure virtual, do something depends on actor
    StudentWorld* getWorld() const { return m_world; } // getter function for m_world
    bool isAlive() { return m_alive; }
    void unalive() { m_alive = false; }
private:
    StudentWorld* m_world; // pointer to the world
    bool m_alive;
};


// --- Person Base Class Implementation ---
// Represents the people in the game -- iceman and the Protesters

class Person : public Actor {
public:
    Person(int imageId, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world, int hp) : Actor(imageId, x, y, dir, size, depth, world), m_hp(hp) {
        setVisible(true);
    }
    virtual ~Person() {}
    virtual void doSomething() = 0;
    // get damaged
    virtual void decreaseHp(int dmg) { m_hp -= dmg; }
    void setHp(int hp) { m_hp = hp; }
    int getHp() { return m_hp; }
    virtual void move(MoveDirection dir);
private:
    int m_hp;
};


// --- Iceman Class Implementation ---
// Represents the player
class Iceman : public Person {
public:
    // Player spawn at (30, 60) by default
    Iceman(StudentWorld* world) : Person(IID_PLAYER, 30, 60, right, 1, 0, world, 10) { // last parameter describes default health (10)
        m_water = 5; // default water
        m_gold = 0; // default gold nuggets
        m_sonar = 1; // default sonar
    }
    // abstract class destructor will handle it
    ~Iceman() {}
    // player control action
    void doSomething() override;
    // break ice mechanics
    void breakIce();
    // player taking damage
    void decreaseHp(int dmg) override;
    // increment gold count
    void pickGold() {m_gold++;}
    // decrement gold count
    void dropGold() {m_gold--;}
    // recharge water gun from water pool
    void pickPool() {m_water += 5;}
    // decrement water count when using water gun
    void useWater() { m_water--; }
    // add sonar count when picking up a sonar 
    void pickSonar() {m_sonar++;}
    // decrease sonar count when using a sonar
    void useSonar() {m_sonar--;}
    // getter functions for player state
    size_t getWater() const {return m_water;}
    size_t getGold() const {return m_gold;}
    size_t getSonar() const {return m_sonar;}


private:
    // player state
    size_t m_water;
    size_t m_gold;
    size_t m_sonar;
};

// --- Protester Class Implementation ---
// Abstract base class for Protesters
class Protester : public Person {
public:
    Protester(int imageId, int x, int y, StudentWorld* world, int hp);
    virtual ~Protester() {}
    virtual void doSomething() = 0;
    virtual void decreaseHp(int dmg) override;
    int isARestTick();
    bool getIsLeaving() { return m_isLeavingField; }
    void setIsLeaving(bool leavingField) { m_isLeavingField = leavingField; }
    unsigned long getNonRestingTicksSinceAnnoyingPlayer() { return m_nonRestingTickSinceAnnoyingPlayer; }
    void resetNonRestingTicksSinceAnnoyingPlayer() { m_nonRestingTickSinceAnnoyingPlayer = 0;  }
    void incrementNonRestingTicksSinceAnnoyingPlayer() { m_nonRestingTickSinceAnnoyingPlayer++; }
    unsigned long getNonRestingTicksSincePerpendicularMove() { return m_nonRestingTicksSincePerpendicularMove; }
    void resetNonRestingTicksSincePerpendicularMove() { m_nonRestingTicksSincePerpendicularMove = 0; }
    void incrementNonRestingTicksSincePerpendicularMove() { m_nonRestingTicksSincePerpendicularMove++; }
    int getNumSquaresToMoveInCurrentDirection() { return m_numSquaresToMoveInCurrentDirections; }
    void setNumSquaresToMoveInCurrentDirection(int numSquaresToMoveInCurrentDirection) { m_numSquaresToMoveInCurrentDirections = numSquaresToMoveInCurrentDirection;  }
    void decrementNumSquaresToMoveInCurrentDirection() { m_numSquaresToMoveInCurrentDirections--; }
    void setTicksToWaitBetweenMoves(int ticksToWaitBetweenMoves) { m_ticksToWaitBetweenMoves = ticksToWaitBetweenMoves; }
    int getStunTicks() { return m_stunTicks; }
    void decrementStunTicks() { m_stunTicks--; }
    void setStunTicks() { m_stunTicks = m_maxStun; }
    void moveToLeaveLocation();
    void moveToPlayer();
    void pickRandomValidDirection();
    MoveDirection getCurrentDirection() { return m_currentDirection; }
    void setCurrentDirection(MoveDirection currentDirection) { m_currentDirection = currentDirection; }
    bool canMoveTo(int x, int y);
    bool canMoveInDirection(MoveDirection dir);
    // function that returns if the protester has direct line of sight of the 
    bool isFacingIceman(int icemanX, int icemanY) const;
    double getEuclidianDistanceTo(int x, int y) const;
    bool doSomethingPreAction();
    bool tryToAnnoyPlayer();
    bool tryDirectLineMoveToPlayer();
    void randomlyMove();
private:
    int m_stunTicks;
    int m_maxStun;
    int m_numSquaresToMoveInCurrentDirections;
    int m_ticksToWaitBetweenMoves;
    bool m_isLeavingField;
    unsigned long m_nonRestingTickSinceAnnoyingPlayer;
    unsigned long m_nonRestingTicksSincePerpendicularMove;
    MoveDirection m_currentDirection;
};

// --- RegularProtester Class Implementation ---
// Represents the regular Protesters
class RegularProtester : public Protester {
public:
    RegularProtester(StudentWorld* world) : Protester(IID_PROTESTER, 60, 60, world, 5) {}
    ~RegularProtester() {}
    void doSomething() override;
private:

};

// --- HardcoreProtester Class Implementation ---
// Represents the harcore Protesters
class HardcoreProtester : public Protester {
public:
    HardcoreProtester(StudentWorld* world, int M) : Protester(IID_HARD_CORE_PROTESTER, 60, 60, world, 20), m_M(M) {}
    ~HardcoreProtester() {}
    void doSomething() override;
    bool tryToTrackPhone();
private:
    // the max distance away a hardcore protester can track iceman's phone
    int m_M;
};




// --- Object Base Class Implementation ---
// For physical objects in the game and map -- like rocks, ice, squirt
class Object : public Actor {
public:
    Object(int imageId, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world) : Actor(imageId, x, y, dir, size, depth, world) {}
    virtual ~Object() {}
    virtual void doSomething() = 0;
private:

};

// --- Ice Class Implementation ---
// Represents the ice on map
class Ice : public Object {
public:
    // 0.25 for size (image is 4x4, but ice should be 1x1), 0 for depth
    Ice(int x, int y, StudentWorld* world) : Object(IID_ICE, x, y, right, 0.25, 3, world) {
        setVisible(true); // display on screen by default
    }
    // abstract class destructor will handle it
    ~Ice() {};
    void doSomething() override {} // sit still like an ice

private:

};

// --- Boulder Class Implementation ---
// Represents physical boulder on map
class Boulder : public Object {
public:
    Boulder(int x, int y, StudentWorld* world) : Object(IID_BOULDER, x, y, down, 1, 1, world) {
        setVisible(true);
    }
    ~Boulder() {};
    void doSomething() override;
private:
    bool stable = true;
    int fallTimer = 0;
};

// --- Squirt Class Implementation ---
// Represents Water Squirt projectile coming from water gun
class Squirt : public Object {
public:
    Squirt(int x, int y, Direction dir, StudentWorld* world) : Object(IID_WATER_SPURT, x, y, dir, 1, 1, world), m_distanceTravelled(0) {
        setVisible(true);
    }
    ~Squirt() {};
    void doSomething() override;
    bool canMoveTo(int x, int y);
private:
    int m_distanceTravelled;
};

// --- Goodies Base Class Implementation ---
// For consumables picked up by the player
class Goodies : public Actor {
public:
    Goodies(int imageId, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world) : Actor(imageId, x, y, dir, size, depth, world) {}
    virtual ~Goodies() {}
    virtual void doSomething() = 0;
    void setMaxLife(size_t t) {maxLifeTime = t;}
protected:
    size_t getTimer() { return life_timer;}
    void incTimer() {life_timer++;}
    size_t getMaxLife() {return maxLifeTime;}
private:
    size_t life_timer = 0;
    size_t maxLifeTime;
};


// --- Pool Class Implementation ---
// Represents water you can pickup -- the source of water refills
class Pool : public Goodies {
public:
    Pool(int x, int y, StudentWorld* world) : Goodies(IID_WATER_POOL, x, y, right, 1, 3, world) {setVisible(true);}
    ~Pool() {}
    void doSomething() override;

private:

};

// --- Oil Class Implementation ---
// Represents oil you can pickup
class OilBarrel : public Goodies {
public:
    OilBarrel(int x, int y, StudentWorld* world) : Goodies(IID_BARREL, x, y, right, 1, 2, world) {}
    ~OilBarrel() {}
    void doSomething() override;

private:
};

// --- Sonar Class Implementation ---
// Represents the sonar kit
class Sonar : public Goodies {
public:
    Sonar(StudentWorld* world) : Goodies(IID_SONAR, 0, 60, right, 1, 2, world) {
        setVisible(true);
    }
    ~Sonar() {}
    void doSomething() override;

private:

};

// --- Gold Class Implementation ---
// Represents gold nuggets you can pickup
class Gold : public Goodies {
public:
    Gold(int x, int y, StudentWorld* world) : Goodies(IID_GOLD, x, y, right, 1, 2, world) {}
    ~Gold() {}
    void doSomething() override;
    void pickable(bool state) {
        picked = state;
        if (picked) setVisible(true);
    }
private:
    bool picked = false;
};





#endif // ACTOR_H_

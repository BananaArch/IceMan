#ifndef ACTOR_H_
#define ACTOR_H_
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#include "GraphObject.h"
#include "GameConstants.h"
#include "SpriteManager.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
// Base class of all actors
class Actor : public GraphObject {
public:
    Actor(int imageID, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world) : GraphObject(imageID, x, y, dir, size, depth), m_world(world) {}
    virtual ~Actor() {}
    virtual void doSomething() = 0; // pure virtual, do something depends on actor
    StudentWorld* getWorld() const {return m_world;} // getter function for m_world
    bool isAlive() {return m_alive;}
    void unalive() {m_alive = false;}
private:
    StudentWorld* m_world; // pointer to the world
    bool m_alive = true;
};


// --- Person Base Class Implementation ---
// Represents the people in the game -- iceman and the protestors

// TODO: Boulder logic
class Person : public Actor {
public:
    Person(int imageId, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world, int hp) : Actor(imageId, x, y, dir, size, depth, world), m_hp(hp) {}
    virtual ~Person() {}
    virtual void doSomething() = 0;
    // get damaged
    virtual void annoyed(int dmg) {m_hp -= dmg;}
    virtual int getHp() {return m_hp;}
private:
    int m_hp;
};


// --- Iceman Class Implementation ---
// Represents the player
class Iceman : public Person {
public:
    // Player spawn at (30, 60) by default
    Iceman(StudentWorld* world) : Person(IID_PLAYER, 30, 60, right, 1, 0, world, 10) { // last parameter describes default health (10
        setVisible(true); // display on screen by default
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
private:
    // player state
    int m_water;
    int m_gold;
    int m_sonar;
};

// --- Protestor Class Implementation ---
// Abstract base class for protestors
class Protestor : public Person {
public:
    Protestor(int imageId, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world, int hp) : Person(imageId, x, y, dir, size, depth, world, hp), m_annoyance(0) {}
    virtual ~Protestor() {}
    virtual void doSomething() = 0;
private:
    int m_annoyance;
};

// --- RegularProtestor Class Implementation ---
// Represents the regular protestors
class RegularProtestor : public Protestor {
public:
    RegularProtestor(int x, int y, StudentWorld* world) : Protestor(IID_PROTESTER, x, y, left, 1, 0, world, 5) {}
    ~RegularProtestor() {}
    void doSomething() override;
private:
    
};

// --- HardcoreProtestor Class Implementation ---
// Represents the harcore protestors
class HardcoreProtestor : public Protestor {
public:
    HardcoreProtestor(int x, int y, StudentWorld* world) : Protestor(IID_HARD_CORE_PROTESTER, x, y, left, 1, 0, world, 20) {}
    ~HardcoreProtestor() {}
    void doSomething() override;
private:

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

// --- Spurt Class Implementation ---
// Represents Water Spurt projectile coming from water gun
class Spurt : public Object {
public:
    Spurt(int x, int y, Direction dir, StudentWorld* world) : Object(IID_WATER_SPURT, x, y, dir, 1, 1, world) {}
    ~Spurt() {};
    void doSomething() override;

private:

};




// --- Goodies Base Class Implementation ---
// For consumables picked up by the player
class Goodies : public Actor {
public:
    Goodies(int imageId, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* world) : Actor(imageId, x, y, dir, size, depth, world) {}
    virtual ~Goodies() {}
    virtual void doSomething() = 0;
private:

};


// --- Pool Class Implementation ---
// Represents water you can pickup -- the source of water refills
class Pool : public Goodies {
public:
    Pool(int x, int y, StudentWorld* world) : Goodies(IID_WATER_POOL, x, y, right, 1, 3, world) {}
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
    Sonar(StudentWorld* world) : Goodies(IID_SONAR, 0, 60, right, 1, 2, world) {}
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

private:

};





#endif // ACTOR_H_

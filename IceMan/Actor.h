#ifndef ACTOR_H_
#define ACTOR_H_
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
#include "GraphObject.h"
#include "GameConstants.h"
#include "SpriteManager.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
// Base class of all actors
class Actor : public GraphObject {
public:
    Actor(int imageID, int x, int y, Direction dir, double size, unsigned int depth) : GraphObject(imageID, x, y, dir, size, depth){}
    virtual ~Actor(){}
    virtual void doSomething() = 0; // pure virtual, do something depends on actor
private:
    
};
// Ice object
class Ice : public Actor {
public:
    // 0.25 for size (image is 4x4, but ice should be 1x1), 0 for depth
    Ice(int x, int y) : Actor(IID_ICE, x, y, right, 0.25, 0) {
        setVisible(true); // display on screen by default
    }
    // abstract class destructor will handle it
    ~Ice() {};
    
    void doSomething() override{} // sit still like an ice
    
private:
    
};
class Iceman : public Actor {
public:
    // Player spawn at (30, 60) by default
    Iceman() : Actor(IID_PLAYER, 30, 60, right, 1, 1) {
        setVisible(true); // display on screen by default
    }
    // abstract class destructor will handle it
    ~Iceman() {};
    // player control action
    void doSomething() {
        
    }
private:
};
#endif // ACTOR_H_

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
    virtual ~Actor(){}
    virtual void doSomething() = 0; // pure virtual, do something depends on actor
private:
	StudentWorld* m_world; // pointer to the world
};
// Ice object
class Ice : public Actor {
public:
    // 0.25 for size (image is 4x4, but ice should be 1x1), 0 for depth
    Ice(int x, int y, StudentWorld* world) : Actor(IID_ICE, x, y, right, 0.25, 3, world) {
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
    Iceman(StudentWorld* world) : Actor(IID_PLAYER, 30, 60, right, 1, 0, world) {
        setVisible(true); // display on screen by default
		hp = 10; // default hp
		water = 5; // default water
		gold = 0; // default gold nuggets
		sonar = 1; // default sonar
    }
    // abstract class destructor will handle it
    ~Iceman() {};
    // player control action
    void doSomething() {}
private:
	// player state
	int hp;
	int water;
	int gold;
	int sonar;
};
#endif // ACTOR_H_

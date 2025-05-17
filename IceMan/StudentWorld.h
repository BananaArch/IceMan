#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include <memory>
#include <string>
#include <vector>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir)
        : GameWorld(assetDir)
    {
    }
    // initialize the game/new level
    virtual int init()
    {
        startWorld();
        return GWSTATUS_CONTINUE_GAME;
    }
    // control each actor every tick
    virtual int move()
    {
        // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
        // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
        
        return GWSTATUS_CONTINUE_GAME;
//        decLives();
//        return GWSTATUS_PLAYER_DIED;
    }
    // clean up when player gets unalives or clears the level
    virtual void cleanUp()
    {
    }
private:
    // pointer of iceman/player
    std::shared_ptr<Iceman> m_player;
    // main 2d array that keep tracks of all actors on the oil field
    std::vector<std::vector<std::shared_ptr<Actor> > > field;
    // 2d array that initialize all the ice objects
    std::shared_ptr<Ice> ices[64][64];
    // auxilery function for init(), see StudentWorld.cpp
    void startWorld();
    // Display game stats (in progress)
    void setDisplayText();
    
};
#endif // STUDENTWORLD_H_

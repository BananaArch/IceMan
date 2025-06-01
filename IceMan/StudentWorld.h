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
    // Constructor
    StudentWorld(std::string assetDir)
        : GameWorld(assetDir)
    {
    }
    // Destructor
    ~StudentWorld() {
        // had runtime error when doing range based loop: Document in Report
        for (int x=0; x<64; x++) {
            for (int y=0; y<64; y++) {
                ices[x][y] = nullptr;
            }
        }
        m_player = nullptr;
        field.clear();
        dsList.clear();
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
        // if player died
        if (m_player && m_player->getHp() == 0) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        setStats();
        for (auto actor : dsList) {
            if (actor->isAlive())
                actor->doSomething();
        }
        // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
        // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
        
        return GWSTATUS_CONTINUE_GAME;
    }
    // clean up when player got unalived or cleared the level
    virtual void cleanUp()
    {
        for (int x=0; x<64; x++) {
            for (int y=0; y<64; y++) {
                ices[x][y] = nullptr;
            }
        }
        m_player = nullptr;
        field.clear();
        dsList.clear();
    }
    // getter function for elements in field
    void getField(int x, int y, std::shared_ptr<Actor>& pos) const {
        if (x>=0 && y>=0 && x<64 && y<64) {
            pos = field[x][y];
            return;
        }
        pos = nullptr;
    }
    // change element in the field
    void setField(int x, int y, std::shared_ptr<Actor> pos) {
        if (x>=0 && y>=0 && x<64 && y<64) {
            field[x][y] = pos;
        }
    }
    // getter function for m_player
    std::shared_ptr<Iceman> getIceman() {return m_player;}
    
    std::shared_ptr<Actor> getIce(int x, int y) {
        return ices[x][y];
    }
    // break the ice in ices
    bool breakIce(int x, int y) {
        if (x<0 || x>63 || y<0 || y>63) {
            return false;
        }
        if (ices[x][y]) {
            ices[x][y]->setVisible(false);
            ices[x][y] = nullptr;
            return true;
        }
        return false;
    }
    
    
private:
    // pointer of iceman/player
    std::shared_ptr<Iceman> m_player;
    // main 2d array that keep tracks of all actors except iceman and ice
    std::vector<std::vector<std::shared_ptr<Actor>>> field;
    // 2d array that initialize all the ice objects
    std::shared_ptr<Ice> ices[64][64];
    // vector that keep tracks of all actors that can doSomething
    std::vector<std::shared_ptr<Actor>> dsList;
    // auxilery function for init(), see StudentWorld.cpp
    void startWorld();
    // Display game stats (in progress)
    void setStats();
    // Add boulders to the game
    void setBoulder();
    // check distance between objects to ensure they're not too close
    bool checkObjectDist(int x, int y);
};
#endif // STUDENTWORLD_H_

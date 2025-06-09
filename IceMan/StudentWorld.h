#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include <memory>
#include <string>
#include <vector>
#include <future>

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
        bList.clear();
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
        ++m_tickCount;

        // if player died
        if (m_player && m_player->getHp() <= 0) {
            playSound(SOUND_PLAYER_GIVE_UP);
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        // if all oil barrels are collected, advance to next level
        if (oList.size() == 0) {
            return GWSTATUS_FINISHED_LEVEL;
        }
        setStats();

        // clean up any dead boulders
        for (auto it = bList.begin(); it != bList.end(); ) {
            if ((*it)->isAlive()) ++it;
            else it = bList.erase(it);
        }

        // clean up any dead oil barrels
        for (auto it = oList.begin(); it != oList.end(); ) {
            if ((*it)->isAlive()) ++it;
            else it = oList.erase(it);
        }
        
        // clean up any dead gold nuggets
        for (auto it = gList.begin(); it != gList.end(); ) {
            if ((*it)->isAlive()) ++it;
            else it = gList.erase(it);
        }
        
        // clean up any dead actors in dsList
        for (auto it = dsList.begin(); it != dsList.end(); ) {
            if ((*it)->isAlive()) ++it;
            else it = dsList.erase(it);
        }
        
        // clean up the field, kinda slow, might fix later
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                if (field[i][j] != nullptr && !field[i][j]->isAlive())
                    field[i][j] = nullptr;
            }
        }
        
        // Launch new pathfinding tasks every 20 ticks
        if (m_tickCount % 20 == 0) {

            playerMapFuture = std::async(std::launch::async, [this]() {
                generatePlayerMap();
                });

            returnMapFuture = std::async(std::launch::async, [this]() {
                generateReturnMap();
                });
        }

        // clean up any protesters
        for (auto it = pList.begin(); it != pList.end(); ) {
            if ((*it)->isAlive()) ++it;
            else it = pList.erase(it);
        }

        // create protesters, if possible
        createProtester();

        // doSomething cycle
        for (auto it = dsList.begin(); it != dsList.end(); ) {
            if ((*it)->isAlive()) { // if actor is alive
                (*it)->doSomething(); // do something
                ++it; // go to next actor if actor is alive
            }
            else { // if actor is not alive
                it = dsList.erase(it); // erase returns the next valid
            } // Notice that we only increment the iterator when we don't erase
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
        pList.clear();
        bList.clear();
        oList.clear();
        gList.clear();
    }
    // getter function for elements in field
    void getField(int x, int y, std::shared_ptr<Actor>& pos) const {
        if (x>=0 && y>=0 && x<64 && y<64) {
            pos = field[x][y];
            return;
        }
        pos = nullptr;
    }
    // change element in the
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
    
    std::vector<std::shared_ptr<Boulder>> getBoulders() {return bList;}
    
    // see if prospective move by a 4x4 person is legal or not based on whether or not there is a boulder blocking it
    bool containsBoulder(int x, int y) {
        for (auto boulder : bList) {
            // Check if the person's prospective coordinates (x, y) falls within the 4x4 area of any boulder
            if ((x + 4) > boulder->getX() && x < boulder->getX() + 4 &&
                (y + 4) > boulder->getY() && y < boulder->getY() + 4) {
                return true;
            }
        }
        return false; // No boulder found occupying the spot (x, y)
    }

    // see if prospective move by a 4x4 protester is legal or not based on whether or not there is ice blocking it
    bool containsIce(int x, int y) {
        for (int i = 0; i <= 64; i++) {
            for (int j = 0; j <= 64; j++) {
                if (ices[i][j] != nullptr) {
                    if (x + 4 > i && x < i + 1 &&
                        y + 4 > j && y < j + 1) return true;
                }
            }
        }
        return false;
    }
    // Add a new gold when player drop it
    void addGold(int x, int y);
    // getter function to allow actors accessing pList
    void getpList(std::vector<std::shared_ptr<Protester>> &copyList) const{
        copyList = pList;
    }
    
    // returns the best direction to move when trying
    MoveDirection bestDirectionToReturn(int x, int y) {
        int bestVal = returnMap[x][y];
        MoveDirection bestDir = moveNone;

        // Check left
        if (x > 0 && returnMap[x - 1][y] < bestVal) {
            bestVal = returnMap[x - 1][y];
            bestDir = moveLeft;
        }
        // Check right
        if (x < 60 && returnMap[x + 1][y] < bestVal) {
            bestVal = returnMap[x + 1][y];
            bestDir = moveRight;
        }
        // Check up
        if (y < 60 && returnMap[x][y + 1] < bestVal) {
            bestVal = returnMap[x][y + 1];
            bestDir = moveUp;
        }
        // Check down
        if (y > 0 && returnMap[x][y - 1] < bestVal) {
            bestVal = returnMap[x][y - 1];
            bestDir = moveDown;
        }

        return bestDir;
    }

    MoveDirection bestDirectionToPlayer(int x, int y) {
        int bestVal = playerMap[x][y];
        MoveDirection bestDir = moveNone;

        // Check left
        if (x > 0 && playerMap[x - 1][y] < bestVal) {
            bestVal = playerMap[x - 1][y];
            bestDir = moveLeft;
        }
        // Check right
        if (x < 60 && playerMap[x + 1][y] < bestVal) {
            bestVal = playerMap[x + 1][y];
            bestDir = moveRight;
        }
        // Check up
        if (y < 60 && playerMap[x][y + 1] < bestVal) {
            bestVal = playerMap[x][y + 1];
            bestDir = moveUp;
        }
        // Check down
        if (y > 0 && playerMap[x][y - 1] < bestVal) {
            bestVal = playerMap[x][y - 1];
            bestDir = moveDown;
        }

        return bestDir;
    }

    int getShortestDistanceToPlayer(int x, int y) {
        return playerMap[x][y];
    }
    
    unsigned long getTickCount() { return m_tickCount; }

private:

    unsigned long m_tickCount = 0;
    unsigned long m_lastProtesterCreated = 0;

    // used for pathfinding multithreading
    std::future<void> playerMapFuture;
    std::future<void> returnMapFuture;

    // pointer of iceman/player
    std::shared_ptr<Iceman> m_player;
    // main 2d array that keep tracks of all actors except iceman and ice
    std::vector<std::vector<std::shared_ptr<Actor>>> field;
    // 2d array that initialize all the ice objects
    std::shared_ptr<Ice> ices[64][64];
    // 2d array for pathfinding to protester target
    int returnMap[61][61];
    // 2d helper array for pathfinding to protester target
    int returnTempMap[61][61];
    // 2d array for pathfinding to player target
    int playerMap[61][61];
    // 2d helper array for pathfinding to player target
    int playerTempMap[61][61];
    // vector that keep tracks of all actors that can doSomething
    std::vector<std::shared_ptr<Actor>> dsList;
    // vector to keep track all boulders
    std::vector<std::shared_ptr<Boulder>> bList;
    // vector to keep track all protesters
    std::vector<std::shared_ptr<Protester>> pList;
    // vector to keep track all oil barrels
    std::vector<std::shared_ptr<OilBarrel>> oList;
    // vector to keep track all gold nuggets
    std::vector<std::shared_ptr<Gold>> gList;
    // auxilery function for init(), see StudentWorld.cpp
    void startWorld();
    // Display game stats
    void setStats();
    // Add boulders to the game
    void setBoulder();
    // Creates protesters
    void createProtester();
    // check distance between objects to ensure they're not too close
    bool checkObjectDist(int x, int y);
    // Add oil to the game
    void setOil();
    // Add gold nugget to the game
    void setGold();
    
    // generate return map for pathfinding
    void generateReturnMap();
    // generate player map for pathfinding
    void generatePlayerMap();

};
#endif // STUDENTWORLD_H_

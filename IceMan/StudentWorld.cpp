#include "StudentWorld.h"
#include <string>
using namespace std;
#include <vector>
GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}
void StudentWorld::startWorld() {
    // intialize 2d array that keep track of all actors
    field.resize(64, std::vector<std::shared_ptr<Actor> >(64, nullptr));
    // set up the ice objects on its own 2d ice array
    for (int x=0; x<64; x++) {
        for (int y=0; y<64; y++) {
            if ((x > 29 && x < 34 && y > 3) || y > 59) {
                ices[x][y] = nullptr;
            }
            else {
                ices[x][y] = std::make_shared<Ice>(x, y);
            }
        }
    }
    //store the ice objects into the main 2d array
    for (int x=0; x<64; x++) {
        for (int y=0; y<64; y++) {
            if (x >=30 && x <= 33 && y>=4 && y<=59) {
                field[x][y] = nullptr;
                 break;
            }
            field[x][y] = ices[x][y];
        }
    }
    // initialize the iceman
    field[30][60] = std::make_shared<Iceman>();
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

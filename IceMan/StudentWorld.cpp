#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}
// Hello World
void StudentWorld::startWorld() {
    // intialize 2d array that keep track of all actors
    field.resize(64, std::vector<std::shared_ptr<Actor>>(64, nullptr));
    // set up the ice objects on its own 2d ice array
    for (int x=0; x<64; x++) {
        for (int y=0; y<64; y++) {
            if ((x > 29 && x < 34 && y > 3) || y > 59) {
                ices[x][y] = nullptr;
            }
            else {
                ices[x][y] = std::make_shared<Ice>(x, y, this);
            }
        }
    }

    // initialize the iceman
    m_player = std::make_shared<Iceman>(this);
    dsList.emplace_back(m_player);
    setBoulder();
    setOil();
}
// TODO: replace with the correct getter functions
void StudentWorld::setStats() {
    int level = getLevel();
    int lives = getLives();
    int health = m_player->getHp() * 10;
    int squirts(0);
    int gold(0);
    int barrelsLeft(0); // Oil left on the field
    int sonar(0);
    string score = to_string(getScore());
    string stats;
    // stat for game level
    stats = ((level<10) ?"Lvl:  " : "Lvl ") + to_string(level);
    // stat for player health bar
    stats += ((health<100)? " Hlth:  " : " Hlth: ") + to_string(health) + "% ";
    // stat for remaining player lives
    stats += "Lives: " + to_string(lives);
    // stat for remaining charges for water gun
    stats += ((squirts<10)? " Wtr:  " : " Wtr: ") + to_string(squirts);
    // stat for number of gold
    stats += " Gld: " + to_string(gold);
    // stat for oil left on the field
    stats += ((barrelsLeft<10) ? " Oil Left: " : " Oil Left:  ") + to_string(barrelsLeft);
    // stat for charges of sonar
    stats += ((sonar<10) ? " Sonar:  " : " Sonar: ") + to_string(sonar);
    // stat for score
    string score_stat = "000000";
    unsigned int j = 6;
    for (int i = score.length(); i >= 0; i--) {
        score_stat[j] = score[i];
        j--;
    }
    
    stats += " Scr: " + score_stat;

    setGameStatText(stats);
}
// create boulders on field
void StudentWorld::setBoulder() {
    int level = getLevel();
    int num = min(level/2 + 2, 9);
    for (int i = 0; i < num; i++) {
        int x , y;
        // reposition if position is invalid
        do {
            x = 10 - (i*2); // rand() % 61;
            y = rand() % 61;
        } while ((x<0 || x>60) || (y<20 || y>56) || (x > 26 && x < 37)
                 || checkObjectDist(x, y));
        
        for (int i = x; i <= x+3; i++) {
            for (int j = y; j <= y+3; j++) {
                breakIce(i, j);
            }
        }
        auto newBoulder = make_shared<Boulder>(x, y, this);
        field[x][y] = newBoulder;
        dsList.emplace_back(newBoulder);
        bList.emplace_back(newBoulder);
    }
}

void StudentWorld::setOil() {
    int level = getLevel();
    int num = 15; // min(level/2 + 2, 9);
    for (int i = 0; i < num; i++) {
        int x , y;
        // reposition if position is invalid
        do {
            x = rand() % 61;
            y = rand() % 61;
        } while ((x<0 || x>60) || (y<0 || y>56) || (x > 26 && x < 37)
                 || checkObjectDist(x, y));

        auto newOil = make_shared<OilBarrel>(x, y, this);
        // field[x][y] = newOil;
        dsList.emplace_back(newOil);
    }
}
// check distance between objects to ensure they're not too close
// **TRUE means not okay, FALSE means the position is fine!**
bool StudentWorld::checkObjectDist(int x, int y) {
    // Check if the object is at the tunnel
    if (x > 29 && x < 34 && y > 3)
        return true;
    
    // Check if objects are 6 units away from each other
    int dist_x, dist_y;
    for (int i = max(x-6, 0); i < min(x+6, 61); i++) {
        for (int j = max(y-6, 0); j < min(y+6, 57); j++) {
            // if objects are kind of close, check!
            if (field[i][j] == nullptr) {
                continue;
            }
            dist_x = field[i][j]->getX() - x;
            dist_y = field[i][j]->getY() - y;
            int distance = dist_x * dist_x + dist_y * dist_y;
            if (distance < 36) {
                return true;
            }
        }
    }
    return false;
}

#include "StudentWorld.h"
#include "Actor.h"
#include <string>
using namespace std;
#include <vector>
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
    field[30][60] = m_player;
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
    stats += ((health<100)? " Hlth:  " : " Hlth: ") + to_string(health) + "%";
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

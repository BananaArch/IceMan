#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

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

    for (int x = 0; x < 61; x++) {
        for (int y = 0; y < 61; y++) {

            returnMap[x][y] = -1;
            playerMap[x][y] = -1;
            
        }
    }
    
    // initialize tickCount
    m_tickCount = 0;


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
            y = std::rand() % 61;
        } while ((x<0 || x>60) || (y<20 || y>56) || (x > 26 && x < 37)
                 || checkObjectDist(x, y));
        
        for (int i = x; i <= x+3; i++) {
            for (int j = y; j <= y+3; j++) {
                breakIce(i, j);
            }
        }
        auto newBoulder = make_shared<Boulder>(x, y, this);
        setField(x, y, newBoulder);
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
            x = std::rand() % 61;
            y = std::rand() % 61;
        } while ((x<0 || x>60) || (y<0 || y>56) || (x > 26 && x < 37)
                 || checkObjectDist(x, y));

        auto newOil = make_shared<OilBarrel>(x, y, this);
        setField(x, y, newOil);
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

void StudentWorld::generateReturnMap() {

    const int destinationX = 60;
    const int destinationY = 60;

    for (int i = 0; i < 61; i++) {
        for (int j = 0; j < 61; j++) {
            if (containsBoulder(i, j) || containsIce(i, j)) { // inaccessible, because boulder or ice
                returnTempMap[i][j] = 1000000;
            }
            else {
                returnTempMap[i][j] = -1;
            }
        }
    }

    std::queue<std::pair<int, int>> q;
    q.push({ destinationX, destinationY });
    returnTempMap[destinationX][destinationY] = 0;

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();
        int x = curr.first;
        int y = curr.second;
        int dist = returnTempMap[x][y];

        // Check Up and Fill
        if (y + 1 <= 60 && returnTempMap[x][y + 1] == -1) {
            returnTempMap[x][y + 1] = dist + 1;
            q.push({ x, y + 1 });
        }
        // Check Down and Fill
        if (y - 1 >= 0 && returnTempMap[x][y - 1] == -1) {
            returnTempMap[x][y - 1] = dist + 1;
            q.push({ x, y - 1 });
        }
        // Check Left and Fill
        if (x - 1 >= 0 && returnTempMap[x - 1][y] == -1) {
            returnTempMap[x - 1][y] = dist + 1;
            q.push({ x - 1, y });
        }
        // Check Right and Fill
        if (x + 1 <= 60 && returnTempMap[x + 1][y] == -1) {
            returnTempMap[x + 1][y] = dist + 1;
            q.push({ x + 1, y });
        }
    }

    // move everything from temp to official
    for (int i = 0; i < 61; ++i)
        for (int j = 0; j < 61; ++j)
            returnMap[i][j] = returnTempMap[i][j];
}

void StudentWorld::generatePlayerMap() {
    int destinationX = m_player->getX();
    int destinationY = m_player->getY();

    for (int i = 0; i < 61; i++) {
        for (int j = 0; j < 61; j++) {
            if (containsBoulder(i, j) || containsIce(i, j)) { // inaccessible, because boulder or ice
                playerTempMap[i][j] = 1000000;
            }
            else {
                playerTempMap[i][j] = -1;
            }
        }
    }

    std::queue<std::pair<int, int>> q;
    q.push({ destinationX, destinationY });
    playerTempMap[destinationX][destinationY] = 0;

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();
        int x = curr.first;
        int y = curr.second;
        int dist = playerTempMap[x][y];

        // Check Up and Fill
        if (y + 1 <= 60 && playerTempMap[x][y + 1] == -1) {
            playerTempMap[x][y + 1] = dist + 1;
            q.push({ x, y + 1 });
        }
        // Check Down and Fill
        if (y - 1 >= 0 && playerTempMap[x][y - 1] == -1) {
            playerTempMap[x][y - 1] = dist + 1;
            q.push({ x, y - 1 });
        }
        // Check Left and Fill
        if (x - 1 >= 0 && playerTempMap[x - 1][y] == -1) {
            playerTempMap[x - 1][y] = dist + 1;
            q.push({ x - 1, y });
        }
        // Check Right and Fill
        if (x + 1 <= 60 && playerTempMap[x + 1][y] == -1) {
            playerTempMap[x + 1][y] = dist + 1;
            q.push({ x + 1, y });
        }
    }
    // move everything from temp to official
    for (int i = 0; i < 61; ++i)
        for (int j = 0; j < 61; ++j)
            playerMap[i][j] = playerTempMap[i][j];
}

void StudentWorld::createProtester() {
    // max ticks that must pass before new protester can be created
    int T = (200 - getLevel() < 25) ? 25 : 200 - getLevel(); // max(25, 200 – current_level_number) 
    // max protesters on field 
    int P = (2 + getLevel() * 1.5 > 15) ? 15 : 2 + getLevel() * 1.5; // min(15, 2 + current_level_number * 1.5)
    if (m_tickCount - m_lastProtesterCreated < T && pList.size() > 0 || pList.size() >= P) return;

    // update last created tick
    m_lastProtesterCreated = m_tickCount;

    // probability of hardcore protester: min(90, getLevel() * 10 + 30)
    int hardcoreProb = (getLevel() * 10 + 30 > 90) ? 90 : getLevel() * 10 + 30;

    // random value from [1, 100] to determine protester type
    int randVal = 1 + std::rand() % 100;

    std::shared_ptr<Protester> protester;

    if (randVal <= hardcoreProb) {
        protester = std::make_shared<HardcoreProtester>(this);
    }
    else {
        protester = std::make_shared<RegularProtester>(this);
    }

    // Add to protester list and do something list
    pList.push_back(protester);
    dsList.push_back(protester);

}
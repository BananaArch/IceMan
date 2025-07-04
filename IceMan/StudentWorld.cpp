#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <future>
#include <mutex>
#include <memory>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}
// create a mutex to avoid race condition
mutex setObjectLock;
void StudentWorld::startWorld() {
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
    // using multi threading to initialize game objects
    auto boulderFuture = std::async(std::launch::async, [this]() {
        this->setBoulder();
    });
    auto oilFuture = std::async(std::launch::async, [this]() {
        this->setOil();
    });
    auto goldFuture = std::async(std::launch::async, [this]() {
        this->setGold();
    });

    // initialize tickCount
    m_tickCount = 0;

    // initialize the iceman
    m_player = std::make_shared<Iceman>(this);

    boulderFuture.get();
    oilFuture.get();
    goldFuture.get();
    unique_lock<mutex> lock(setObjectLock);
    dsList.emplace_back(m_player);
    // TODO: Use multithreading to generate maps

    generatePlayerMap();
    generateReturnMap();


}
// set stats for scoreboard
void StudentWorld::setStats() {
    int level = getLevel();
    int lives = getLives();
    int health = m_player->getHp() * 10;
    size_t squirts = m_player->getWater();
    size_t gold = m_player->getGold();
    int barrelsLeft = oList.size(); // Oil left on the field
    size_t sonar = m_player->getSonar();
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
    unique_lock<mutex> lock(setObjectLock);
    int level = getLevel();
    int num = min(level/2 + 2, 9);
    for (int i = 0; i < num; i++) {
        int x , y;
        // reposition if position is invalid
        do {
            x = std::rand() % 61;
            y = std::rand() % 61;
        } while ((x<0 || x>60) || (y<20 || y>56) || (x > 26 && x < 37)
                 || checkObjectDist(x, y));
        
        for (int i = x; i <= x+3; i++) {
            for (int j = y; j <= y+3; j++) {
                breakIce(i, j);
            }
        }
        auto newBoulder = make_shared<Boulder>(x, y, this);
        dsList.emplace_back(newBoulder);
        bList.emplace_back(newBoulder);
    }
}
// Add oil to the game
void StudentWorld::setOil() {
    unique_lock<mutex> lock(setObjectLock);
    int level = getLevel();
    int num = min(2 + level, 21);
    for (int i = 0; i < num; i++) {
        int x , y;
        // reposition if position is invalid
        do {
            x = std::rand() % 61;
            y = std::rand() % 61;
        } while ((x<=0 || x>=60) || (y<=0 || y>=56) || (x > 26 && x < 37)
                 || checkObjectDist(x, y));

        auto newOil = make_shared<OilBarrel>(x, y, this);
        dsList.emplace_back(newOil);
        oList.emplace_back(newOil);
    }
}
// Add gold to the game
void StudentWorld::setGold() {
    unique_lock<mutex> lock(setObjectLock);
    int level = getLevel();
    int num = max(5 - level, 2);
    for (int i = 0; i < num; i++) {
        int x , y;
        // reposition if position is invalid
        do {
            x = std::rand() % 61;
            y = std::rand() % 61;
        } while ((x<=0 || x>=60) || (y<=0 || y>=56) || (x > 26 && x < 37)
                 || checkObjectDist(x, y));

        auto newGold = make_shared<Gold>(x, y, this);
        dsList.emplace_back(newGold);
        gList.emplace_back(newGold);
    }
}
void StudentWorld::addGold(int x, int y) {
    auto newGold = make_shared<Gold>(x, y, this);
    newGold->setMaxLife(100);
    newGold->pickable(true);
    dsList.emplace_back(newGold);
    gList.emplace_back(newGold);
    
}
// check distance between objects to ensure they're not too close
// **TRUE means not okay, FALSE means the position is fine!**
bool StudentWorld::checkObjectDist(int x, int y) {
    // Check if the object is at the tunnel
    if (x > 29 && x < 34 && y > 3)
        return true;
    
    // Check if boulders, gold and oil barrels are 6 units away from each other
    int dist_x, dist_y;
    for (auto oil: oList) {
        dist_x = oil->getX() - x;
        dist_y = oil->getY() - y;
        int distance = dist_x * dist_x + dist_y * dist_y;
        if (distance < 36) return true;
    }
    for (auto gold: gList) {
        dist_x = gold->getX() - x;
        dist_y = gold->getY() - y;
        int distance = dist_x * dist_x + dist_y * dist_y;
        if (distance < 36) return true;
    }
    for (auto bould: bList) {
        dist_x = bould->getX() - x;
        dist_y = bould->getY() - y;
        int distance = dist_x * dist_x + dist_y * dist_y;
        if (distance < 36) return true;
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
    int T = (200 - getLevel() < 25) ? 25 : 200 - getLevel(); // max(25, 200 � current_level_number)
    // max protesters on field
    int P = (2 + getLevel() * 1.5 > 15) ? 15 : 2 + getLevel() * 1.5; // min(15, 2 + current_level_number * 1.5)
    if (m_tickCount - m_lastProtesterCreated < T && pList.size() > 0 || pList.size() >= P) return;
    int M = 16 + getLevel() * 2;

    // update last created tick
    m_lastProtesterCreated = m_tickCount;

    // probability of hardcore protester: min(90, getLevel() * 10 + 30)
    int hardcoreProb = (getLevel() * 10 + 30 > 90) ? 90 : getLevel() * 10 + 30;

    // random value from [1, 100] to determine protester type
    int randVal = 1 + std::rand() % 100;

    std::shared_ptr<Protester> protester;

    if (randVal <= hardcoreProb) {
        protester = std::make_shared<HardcoreProtester>(this, M);
    }
    else {
        protester = std::make_shared<RegularProtester>(this);
    }

    // Add to protester list and do something list
    pList.push_back(protester);
    dsList.push_back(protester);

}

void StudentWorld::addTools() {
    // decide whether adding a tool on the field or not
    bool adding = false;
    int chance = getLevel() * 30 + 290;
    if (std::rand() % chance == 69) {
        adding = true;
    }
    // decide between adding water pool or sonar kit
    if (adding) {
        if (std::rand() % 5 > 0) {
            addPool();
        }
        else {
            addSonar();
        }
    }
}

void StudentWorld::addPool() {
    int x , y;
    bool isIce;

    // look for a position that has no ice and spawn pool
    do {
        isIce = false;
        x = std::rand() % 61;
        y = std::rand() % 61;
        
        for (int i = x; i <= x+3; i++) {
            for (int j = y; j <= y+3; j++) {
                if (ices[i][j]) {
                    isIce = true;
                }
            }
        }
        // to avoid overlapping with sonar
        if (x==0 && y==60)
            isIce = true;
    } while(isIce);
    // adding pool to the game
    auto newPool = make_shared<Pool>(x, y, this);
    int m = 300 - 10*getLevel();
    newPool->setMaxLife(max(100, m));
    dsList.emplace_back(newPool);
    tList.emplace_back(newPool);
}

void StudentWorld::addSonar() {
    // start at x=0, y=60
    bool already = false;
    for (auto tool: tList) {
        if (tool->getX() == 0 && tool->getY() == 60) already = true;
    }
    if (!already) {
        auto newSonar = make_shared<Sonar>(this);
        int m = 300 - 10*getLevel();
        newSonar->setMaxLife(max(100, m));
        dsList.emplace_back(newSonar);
        tList.emplace_back(newSonar);
    }
}

void StudentWorld::sonarScan() {
    int x = m_player->getX();
    int y = m_player->getY();
    for (auto object: dsList) {
        int distX = x - object->getX();
        int distY = y - object->getY();
        if ((distX*distX + distY*distY) <= 144)
            object->setVisible(true);
    }
}

// returns a vector with smart ptrs to all protesters at a specific position
vector<shared_ptr<Protester>> StudentWorld::getOverlappingProtesters(int x, int y) {
    std::vector<std::shared_ptr<Protester>> overlappingProtesters;
    for (const auto& protester : pList) {
        if (x + 4 > protester->getX() && x < protester->getX() + 4 &&
            y + 4 > protester->getY() && y < protester->getY() + 4) {
            overlappingProtesters.push_back(protester);
        }
    }
    return overlappingProtesters;
}

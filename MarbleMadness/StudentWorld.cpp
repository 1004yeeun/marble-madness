#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}//$

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_avatar = nullptr; //initializing world and pointing m_avatar to nothing
    actors.clear(); //initializing world and emptying vector
}//$

int StudentWorld::init()
{
    //cleaning everything to base before building level//
    m_avatar = nullptr; //making sure m_avatar points to nothing
    actors.clear(); //making sure actors vector is empty
    setLevelCompletion(false); //initializing so level is incomplete at start
    //if (getLevel()==4) { setLevelCompletion(false); }
    setCrystals(); //making sure there are zero crystals counted
    setBonus(); //making sure the bonus starts at 1000
    
    string curLevel = "level0" + to_string(getLevel()) + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found) {
        cerr << "Cannot find " + curLevel + " data file" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format) {
        cerr << "Level file is not correctly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success) {
        cerr << "Level file was successfully loaded" << endl;
        for (int x=0; x<VIEW_WIDTH; x++) {
            for (int y=0; y<VIEW_HEIGHT;y++) {
                if (isValidPos(x,y)) {
                    switch( lev.getContentsOf(x,y) ) {
                        case Level::empty:
                            cerr << to_string(x) + "," + to_string(y) + " should be empty in the maze" << endl;
                            break;
                        case Level::exit:
                            cerr << to_string(x) + "," + to_string(y) + " should be an exit in the maze" << endl;
                            actors.push_back(new Exit(this,x,y));
                            break;
                        case Level::player:
                            cerr << to_string(x) + "," + to_string(y) + " should be a player in the maze" << endl;
                            m_avatar = new Avatar(this,x,y);
                            actors.push_back(m_avatar);
                            break;
                        case Level::horiz_ragebot:
                            cerr << to_string(x) + "," + to_string(y) + " should be a horiz_ragebot in the maze" << endl;
                            actors.push_back(new RageBot(this,x,y,0));
                            break;
                        case Level::vert_ragebot:
                            cerr << to_string(x) + "," + to_string(y) + " should be a vert_ragebot in the maze" << endl;
                            actors.push_back(new RageBot(this,x,y,270));
                            break;
                        case Level::thiefbot_factory:
                            cerr << to_string(x) + "," + to_string(y) + " should be a thiefbot_factory in the maze" << endl;
                            actors.push_back(new ThiefBotFactory(this,x,y,2));
                            break;
                        case Level::mean_thiefbot_factory:
                            cerr << to_string(x) + "," + to_string(y) + " should be a mean_thiefbot_factory in the maze" << endl;
                            actors.push_back(new ThiefBotFactory(this,x,y,3));
                            break;
                        case Level::wall:
                            cerr << to_string(x) + "," + to_string(y) + " should be a wall in the maze" << endl;
                            actors.push_back(new Wall(this,x,y));
                            break;
                        case Level::marble:
                            cerr << to_string(x) + "," + to_string(y) + " should be a marble in the maze" << endl;
                            actors.push_back(new Marble(this,x,y));
                            break;
                        case Level::pit:
                            cerr << to_string(x) + "," + to_string(y) + " should be a pit in the maze" << endl;
                            actors.push_back(new Pit(this,x,y));
                            break;
                        case Level::crystal:
                            cerr << to_string(x) + "," + to_string(y) + " should be a crystal in the maze" << endl;
                            addCrystal();
                            actors.push_back(new Crystal(this,x,y));
                            break;
                        case Level::restore_health:
                            cerr << to_string(x) + "," + to_string(y) + " should be a restore_health in the maze" << endl;
                            actors.push_back(new RestoreHealthGoodie(this,x,y));
                            break;
                        case Level::extra_life:
                            cerr << to_string(x) + "," + to_string(y) + " should be a extra_life in the maze" << endl;
                            actors.push_back(new ExtraLifeGoodie(this,x,y));
                            break;
                        case Level::ammo:
                            cerr << to_string(x) + "," + to_string(y) + " should be ammo in the maze" << endl;
                            actors.push_back(new AmmoGoodie(this,x,y));
                            break;
                    }
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}//$

int StudentWorld::move()
{
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << getScore();
    oss << "  Level: " << setw(2) << getLevel();
    oss.fill(' ');
    oss << "  Lives: " << setw(2) << getLives();
    oss << "  Health: " << setw(3);
    oss << m_avatar->getHealthPercentage();
    oss << "%  Ammo: " << setw(3) << m_avatar->getPeas();
    oss << "  Bonus: " << setw(4) << getBonus();
    setGameStatText(oss.str());
    
    for (int i=0; i<actors.size();i++) {
        if (actors[i]->isAlive()) { actors[i]->doSomething(); }
        //It must ask all of the actors that are currently active in the game world to do something
        if (!m_avatar->isAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        } //If an actor does something that causes the player to die, then the move() method should immediately return GWSTATUS_PLAYER_DIED.
        if (isLevelCompleted()) {
            increaseScore(2000+getBonus());
            return GWSTATUS_FINISHED_LEVEL;
        } //If the player steps onto the same square as an exit (after first collecting all of the crystals on the level), completing the current level, then the move() method should immediately:
        //i.Increase the player’s score appropriately (by 2000 points for using the exit, and by the remaining bonus score for the level).
        //ii.Return a value of GWSTATUS_FINISHED_LEVEL.
    }
    
    //It must then delete any actors that have died during this tick
    for (vector<Actor*>::iterator it = actors.begin(); it!=actors.end();) {
        if (!((*it)->isAlive())) {
            delete *it;
            vector<Actor*>::iterator p = actors.erase(it);
            it = p;
        }
        else { it++; }
    }

    
    decBonus(); //decreases the bonus by a point per tick
    
    if (!(m_avatar->isAlive())) {
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (isLevelCompleted()) {
        increaseScore(2000+getBonus());
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}//$

void StudentWorld::cleanUp()
{
    for (int i=0; i<actors.size(); i++) {
        delete actors[i];
    }
    actors.clear();
    m_avatar = nullptr;
    setCrystals();
}//$

bool StudentWorld::peaDoesDamage(int x, int y) {
    for (vector<Actor*>::iterator it=actors.begin(); it!=actors.end(); it++) {
        if ((*it)->isAlive() && (*it)->isDamageable()) {
            if ((*it)->getX()==x && (*it)->getY()==y) {
                (*it)->loseHealth();
                return true;
            }
        }
    }
    return false;
}//$

bool StudentWorld::blocksPea(int x, int y) {
    for (vector<Actor*>::iterator it=actors.begin(); it!=actors.end(); it++) {
        if ((*it)->isAlive() && (*it)->stopsPeas()) {
            if ((*it)->getX()==x && (*it)->getY()==y) {
                return true;
            }
        }
    }
    return false;
}//$

void StudentWorld::addPea(int x, int y, int dir) {
    actors.push_back(new Pea(this,x,y,dir));
    actors.back()->setDirection(dir);
}//$

bool StudentWorld::isValidPos(int x, int y) {
    return isValidPos(x,y,nullptr);
}//$

bool StudentWorld::isValidPos(int x, int y, Actor* actor) {
    for (auto a : actors) {
        if (a != actor) {
            if (a->isAlive() && !(a->allowsAgentColocation())) {
                if (a->getX()==x && a->getY()==y) { return false; }
            }
        }
    }
    return true;
}

bool StudentWorld::isMarble(int x, int y) {
    for (vector<Actor*>::iterator it=actors.begin(); it!=actors.end(); it++) {
        if ((*it)->isAlive() && (*it)->isSwallowable()) {
            if ((*it)->getX()==x && (*it)->getY()==y) { return true; }
        }
    }
    return false;
}//$

Actor* StudentWorld::getActorAt(int x, int y) { //return a ptr to actor at x,y
    if ((x==(m_avatar->getX())) && (y==(m_avatar->getY()))) { return m_avatar; }
    for (int i=0; i<actors.size(); i++) {
        if ((actors[i]->getX()==x) && (actors[i]->getY()==y)) {
            return actors[i];
        }
    }
    return nullptr;
}//$

bool StudentWorld::foundTargetInRange(int x, int y, int dir) {
    Actor* tempActor;
    switch (dir) {
        case 0: //right
            for (int i=1; i<(16-x); i++) {
                tempActor = getActorAt(x+i,y);
                if (tempActor == nullptr) { continue; }
                if (tempActor == m_avatar) { return true; }
                if (tempActor->blocksBotRange()) { return false; }
            }
            break;
        case 90: //up
            for (int i=1; i<(16-y); i++) {
                tempActor = getActorAt(x,y+i);
                if (tempActor == nullptr) { continue; }
                if (tempActor == m_avatar) { return true; }
                if (tempActor->blocksBotRange()) { return false; }
            }
            break;
        case 180: //left
            for (int i=1; (x-i)>0; i++) {
                tempActor = getActorAt(x-i,y);
                if (tempActor == nullptr) { continue; }
                if (tempActor == m_avatar) { return true; }
                if (tempActor->blocksBotRange()) { return false; }
            }
            break;
        case 270: //down
            for (int i=1; (y-i)>0; i++) {
                tempActor = getActorAt(x,y-i);
                if (tempActor == nullptr) { continue; }
                if (tempActor == m_avatar) { return true; }
                if (tempActor->blocksBotRange()) { return false; }
            }
            break;
        default:
            break;
    }
    return false;
}//$$$$$$$$%%%%%$$$$$$$VCONDENSE$$$$$$%%%%%%%%%%$$$$$$$$$$

bool StudentWorld::isGoodie(int x, int y, int& goodieType) {
    for (int i=0; i<actors.size(); i++) { //iterate through the actor class
        if (actors[i]->isStealable()) { //if it is stealable (a goodie)
            if ((actors[i]->getX()==x) && (actors[i]->getY()==y)) { //if x,y is the same its on a goodie
                if (actors[i]->givesAmmo()) { goodieType = 10; } //reference goodie type by 10,20,30
                if (actors[i]->givesLife()) { goodieType = 20; }
                if (actors[i]->givesHealth()) { goodieType = 30; }
                return true; //return true if it is a goodie
            }
        }
    }
    return false; //return false if it is not a goodie
}//$

void StudentWorld::returnAGoodie(int x, int y, int goodieType) {
    switch (goodieType) {
        case 10:
            actors.push_back(new AmmoGoodie(this,x,y));
            break;
        case 20:
            actors.push_back(new ExtraLifeGoodie(this,x,y));
            break;
        case 30:
            actors.push_back(new RestoreHealthGoodie(this,x,y));
            break;
        default:
            break;
    }
}

int StudentWorld::surroundingCount(int x, int y) { //count how many thiefbots there currently are in 3 coord up,down,left,and right of the factory's pos at x,y
    int count = 0;
    int xLeftMax = x-3;
    int xRightMax = x+3;
    int yUpMax = y+3;
    int yDownMax = y-3;
    for (int i=0; i<actors.size(); i++) { //check if each actor is a thiefbot
        if (actors[i]->countsForFactoryCount()) {
            for (int xi=xLeftMax; xi<=xRightMax; xi++) {
                for (int yi=yDownMax; yi<=yUpMax; yi++) {
                    if (actors[i]->getX()==xi && actors[i]->getY()==yi) { count++; }
                }
            }
        }
    }
    return count;
}

void StudentWorld::addRegularThiefBot(int x, int y) {
    actors.push_back(new RegularThiefBot(this,x,y));
}

void StudentWorld::addMeanThiefBot(int x, int y) {
    actors.push_back(new MeanThiefBot(this,x,y));
}

bool StudentWorld::isThiefBotAt(int x, int y) { //return a bool checking whether there is a thiefbot next to the factory
    for (int i=0; i<actors.size(); i++) {
        if (actors[i]->countsForFactoryCount()) {
            if (actors[i]->getX()==x && actors[i]->getY()==y) {
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::playerIsStandingOn(Actor* actor) {
    if (m_avatar->isAlive()) {
        if ((actor->getX()==m_avatar->getX()) && (actor->getY()==m_avatar->getY())) {
            return true;
        }
    }
    return false;
}

void StudentWorld::giveAmmo() { m_avatar->incPeas(20); }
void StudentWorld::giveHealth() { m_avatar->setHealth(20); }

void StudentWorld::getAvatarPos(int& posX, int& posY) {
    posX = m_avatar->getX();
    posY = m_avatar->getY();
}

bool StudentWorld::isValidBotTarget(int x, int y) {
    return isValidBotTarget(x,y,nullptr);
}

bool StudentWorld::isValidBotTarget(int x, int y, Actor* actor) {
    for (auto a : actors) {
        if (a != actor) {
            if (!(a->isAlive()) && (a->blocksBotRange())) {
                if (a->getX()==x && a->getY()==y) { return false; }
            }
        }
    }
    return true;
}

bool StudentWorld::isValidMarblePos(int x, int y) {
    for (vector<Actor*>::iterator it=actors.begin(); it!=actors.end(); it++) {
        if ((*it)->isAlive() && !((*it)->allowsMarbleColocation())) {
            if ((*it)->getX()==x && (*it)->getY()==y) {
                return false;
            }
        }
    }
    return true;
}

bool StudentWorld::moveMarble(int x, int y, int dir) {
    switch (dir) {
        case 0:
            if ( isValidMarblePos(x+1,y) ) {
                getActorAt(x,y)->moveTo(x+1,y);
                if ( isValidPos(x,y) ) {
                    return true;
                }
            }
            break;
        case 90:
            if ( isValidMarblePos(x,y+1) ) {
                getActorAt(x,y)->moveTo(x,y+1);
                if ( isValidPos(x,y) ) {
                    return true;
                }
            }
            break;
        case 180:
            if ( isValidMarblePos(x-1,y) ) {
                getActorAt(x,y)->moveTo(x-1,y);
                if ( isValidPos(x,y) ) {
                    return true;
                }
            }
            break;
        case 270:
            if ( isValidMarblePos(x,y-1) ) {
                getActorAt(x,y)->moveTo(x,y-1);
                if ( isValidPos(x,y) ) {
                    return true;
                }
            }
            break;
    }
    return false;
}

bool StudentWorld::isPit(int x, int y) {
    for (vector<Actor*>::iterator it=actors.begin(); it!=actors.end(); it++) {
        if ((*it)->isAlive() && (*it)->allowsMarble()) {
            if ((*it)->getX()==x && (*it)->getY()==y) {
                return true;
            }
        }
    }
    return false;
}

Actor* StudentWorld::getPit(int x, int y) {
    for (vector<Actor*>::iterator it=actors.begin(); it!=actors.end(); it++) {
        if ((*it)->isAlive() && (*it)->allowsMarble()) {
            if ((*it)->getX()==x && (*it)->getY()==y) {
                return *it;
            }
        }
    }
    return nullptr;
}

void StudentWorld::killPit(int x, int y) {
    getPit(x,y)->setDead();
}


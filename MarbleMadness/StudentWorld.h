#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Level.h"
#include <vector>
#include <string>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cppFINAL
class Avatar;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init(); //initializes the level file and sets up the level
    virtual int move(); //called for every tick, making every object do something
    virtual void cleanUp(); //clears all actors and the m_avatar pointer
    ~StudentWorld() { cleanUp(); } //destructor using "helper function"
    
    //bonus points
    void setBonus() { m_bonus = 1000; } //starts at 1000
    void decBonus() { if (m_bonus>0) m_bonus--; }
    //decreases the m_bonus by a point per tick
    unsigned int getBonus() { return m_bonus; }
    //returns the value of m_bonus
    
    //level completion
    bool isLevelCompleted () { return m_levelCompleted; }
    void setLevelCompletion(bool hm) { m_levelCompleted = hm; }
    
    //finding crystals
    void foundACrystal() { m_crystal--; }
    //decrease the number of crystals to find when finding one
    bool foundAllCrystals() { return m_crystal==0; }
    //if you found all of the crystals, return true
    void setCrystals() { m_crystal = 0; }
    //set the number of crystals to find to zero
    void addCrystal() { m_crystal++; }
    //add a crystal to find each time one is added to the level
    
    //checking for marbles at x,y
    bool isMarble(int x, int y); //if it is a marble return true
    bool moveMarble(int x, int y, int dir); //move the marble one position from x,y in dir
    bool isValidMarblePos(int x, int y); //if a marble can move here (NOT if it is a marble)
    Actor* getActorAt(int x, int y); //use to get a ptr to actor at x,y
    
    //find a damageable target to shoot
    bool foundTargetInRange(int x, int y, int dir);
    
    //adding peas
    void addPea(int x, int y, int dir);
    //add the pea to the vector, bring it to life!
    bool blocksPea(int x, int y);
    bool peaDoesDamage(int x, int y);
    
    //checking for valid position that is in the 15x15
    bool isValidPos(int x, int y);
    bool isValidPos(int x, int y, Actor* actor);
    
    void returnAGoodie(int x, int y, int goodieType);
    //finds a goodie, and returning the goodietype and its coordinates
    bool isGoodie(int x, int y, int& goodieType);
    //determines if x,y is a goodie and returns goodie type if true

    //for robot factory
    int surroundingCount(int x, int y);
    void addRegularThiefBot(int x, int y);
    void addMeanThiefBot(int x, int y);
    bool isThiefBotAt(int x, int y);

    void getAvatarPos(int& posX, int& posY);
    bool isValidBotTarget(int x, int y, Actor* actor);
    bool isValidBotTarget(int x, int y);
    
    bool playerIsStandingOn(Actor* actor);
    void giveAmmo();
    void giveHealth();
        
    bool isPit(int x, int y);
    void killPit(int x, int y);
    Actor* getPit(int x, int y);
private:
    Avatar* m_avatar;
    vector<Actor*> actors;
    bool m_levelCompleted;
    int m_crystal;
    unsigned int m_bonus;
};

#endif // STUDENTWORLD_H_

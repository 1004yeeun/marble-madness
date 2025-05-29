#ifndef ACTOR_H_
#define ACTOR_H_

//You must create a class to represent the player in the game.

//You must create classes for Horizontal/Vertical RageBots, Regular ThiefBots,
//Mean ThiefBots, factories, marbles, pits, walls, crystals, extra life goodies, restore
//health goodies, ammo Goodies, walls, and the exit, as well as any additional base
//classes (e.g., a robot base class if you find it convenient) that help you implement
//the game.

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject {
public:
    Actor(StudentWorld* sw, int imageID, int startX, int startY, int startDir=0) : GraphObject(imageID, startX, startY, startDir), m_sw(sw), m_isAlive(true), m_isStolen(false), m_hitPoints(1), m_hasStolenAGoodie(false), m_goodieType(0) {}
    virtual ~Actor() {}
    
    //basic getters and setters checking if is alive and sw
    StudentWorld* getWorld() { return m_sw; }
    bool isAlive() { return m_isAlive; }
    virtual void setDead() { m_isAlive = false; }
    //basic getter for sw (percent version of health)

    //mainly regarding marbles and agents
    double getHealthPercentage() { return (getHealth()/20.0) * 100; }
    void setHealth(int num) { m_hitPoints = num; } //initialize health at constr
    int getHealth() { return m_hitPoints; } //get current health
    virtual void loseHealth() {
        m_hitPoints -=2;
        if (getHealth()<=0) { setDead(); }
    }
    //decrease health (same for all but plays different sound)
    
    //for thiefbots and their goodies
    void setGoodieType(int goodieType) { m_goodieType = goodieType; }
    //set the m_goodieType to a specific goodieType indicator
    bool getStealingStatus() { return m_hasStolenAGoodie; }
    //return whether the actor is carrying a goodie or not
    void stoleAGoodie() { m_hasStolenAGoodie = true; }
    //if the actor stole a goodie, set true
    void setStolen() { m_isStolen = true; }
    bool isItStolen() { return m_isStolen; }
    int getGoodieType() { return m_goodieType; }
    //get an integer indicator of what goodie was stolen
    
    //pure virtual indicators (must be specified for each character)
    virtual bool allowsAgentColocation() const = 0;
    virtual void doSomething() = 0;
    
    //indicators
    virtual bool isDamageable() const { return false; } //if a pea colocates, will it be damaged?
    virtual bool stopsPeas() const { return false; } //stops pea if before actor to attack not found
    virtual bool isSwallowable() const { return false; } //only marbles are swallowable (by pits)
    virtual bool blocksBotRange() const { return true; } //to shoot an avatar (ends range)
    //for goodies and specifiers
    virtual bool isStealable() const { return false; }
    virtual bool givesLife() const { return false; }
    virtual bool givesHealth() const { return false; }
    virtual bool givesAmmo() const { return false; }
    //if this object colocates another, can they steal it? (thiefbot indi)
    virtual bool canSteal() { return false;}
    virtual bool allowsMarble() const { return false; }
    virtual bool allowsMarbleColocation() const { return false; }
    virtual bool countsForFactoryCount() const { return false; }
private:
    StudentWorld* m_sw;
    bool m_isAlive;
    int m_hitPoints;
    bool m_isStolen;
    bool m_hasStolenAGoodie;
    int m_goodieType;
};

class Agent : public Actor {
public:
    Agent(StudentWorld* sw, int imageID, int startX, int startY, int dir=0) : Actor(sw, imageID, startX, startY, dir) {}
    virtual ~Agent() {}
    
    //same for robots and actors
    bool move(int dir);
    void getNewPos(int dir, int& newX, int& newY);
    bool canMove(int dir);
    
    //pure virtual functions
    virtual bool checkPosForMarble(int dir) = 0;
    
    //indicators
    virtual bool allowsAgentColocation() const { return false; }
    virtual bool isDamageable() const { return true; } //if a pea colocates, will it be damaged?
    
    //push a pea into the world
    virtual void pushPea(int dir);
};

class Avatar : public Agent {
public:
    Avatar(StudentWorld* sw, int startX, int startY) : Agent(sw, IID_PLAYER, startX, startY), m_peas(20) {
        setHealth(20);
    }
    virtual ~Avatar() {}
    
    //virtual function implementations
    virtual void doSomething();
    virtual bool checkPosForMarble(int dir);
    
    //loses health the same but plays a different sound
    virtual void loseHealth();
    
    //use its peas to possibly attack something
    void incPeas(int amt) { m_peas+=amt; }
    void decPea() { m_peas--; }
    int getPeas() { return m_peas; }
    //call sw to add a new pea actor to sw at x,y
    
    //indicators
    virtual bool blocksBotRange() const { return false; } //to shoot an avatar (ends range)
    virtual bool allowsMarble() const { return true; }
private:
    int m_peas;
};//$

class Pea : public Actor {
public:
    Pea(StudentWorld* sw, int startX, int startY, int dir) : Actor(sw, IID_PEA, startX, startY, dir), m_goingToDie(false), m_firstSpot(true) {}
    virtual ~Pea() {}
    
    //temp bools and getters
    void goingToDieSoon() { m_goingToDie = true; }
    bool willItDie() { return m_goingToDie; }
    //indicating the first position a pea is visibly spawned
    bool firstSpot() { return m_firstSpot; }
    void setFirstSpot() { m_firstSpot = false; }
    
    //virtual function implementations
    virtual void doSomething();
    
    //indicator
    virtual bool allowsAgentColocation() const { return true; };
    virtual bool blocksBotRange() const { return false; } //to shoot an avatar (ends range)
    
private:
    bool m_goingToDie;
    bool m_firstSpot;
};

class Robot : public Agent {
public:
    Robot(StudentWorld* sw, int imageID, int startX, int startY, int dir=0) : Agent(sw, imageID, startX, startY, dir), m_ticks(0) {}
    virtual ~Robot() {}
    
    //virtual function implementations
    virtual bool checkPosForMarble(int dir) { return false; }
    virtual void changeDirection(int dir);
    virtual bool shouldIShoot();
    
    //getter setter functions for managing tick count
    int getTicks() { return m_ticks; }
    void addTicks() { m_ticks++; }
    void resetTicks() { m_ticks = 0; }
    bool timeToAct(); //if m_ticks==getTicks(), else inc tick count
private:
    int m_ticks;
};

class RageBot : public Robot {
public:
    RageBot(StudentWorld* sw, int startX, int startY, int dir) : Robot(sw, IID_RAGEBOT, startX, startY, dir) {
        setHealth(10);
    }
    virtual ~RageBot() {}
    
    //virtual function implementations
    virtual void doSomething();
    virtual void loseHealth();
};

class ThiefBot : public Robot {
public:
    ThiefBot(StudentWorld* sw, int imageID, int startX, int startY) : Robot(sw, imageID, startX, startY), m_distanceBeforeTurning(6), m_distanceTicker(0) {}
    virtual ~ThiefBot() {}
    
    //if this object colocates another, can they steal it? (thiefbot indi)
    virtual bool canSteal() { return true; }

    //virtual function implementations
    virtual void doSomething();
    
    //getters and setters for distance ticker
    int getDistanceBeforeTurning() { return m_distanceBeforeTurning; }
    int getDistanceTicker() { return m_distanceTicker; }
    void setNewDistanceBeforeTurning() { m_distanceBeforeTurning = (rand()&6)+1; }
    void resetDistanceTicker() { m_distanceTicker = 0; }
    void incDistanceTicker() { m_distanceTicker++; }
    
    //indicator
    virtual bool countsForFactoryCount() const { return true; }
private:
    int m_distanceBeforeTurning;
    int m_distanceTicker;
};

class MeanThiefBot : public ThiefBot {
public:
    MeanThiefBot(StudentWorld* sw, int startX, int startY) : ThiefBot(sw, IID_MEAN_THIEFBOT, startX, startY) {
        setHealth(8);
    }
    virtual ~MeanThiefBot() {}
    
    //virtual function implementations
    virtual void loseHealth();
};

class RegularThiefBot : public ThiefBot {
public:
    RegularThiefBot(StudentWorld* sw, int startX, int startY) : ThiefBot(sw, IID_THIEFBOT, startX, startY) {
        setHealth(5);
    }
    virtual ~RegularThiefBot() {}
    
    //virtual function implementations
    virtual bool shouldIShoot() { return false; } //this robot is not mean.
    virtual void loseHealth();
};

class Wall : public Actor {
public:
    Wall(StudentWorld* sw, int startX, int startY) : Actor(sw, IID_WALL, startX, startY, none) {}
    virtual ~Wall() {}
    
    //virtual function implementations
    virtual void doSomething() { return; }
    
    //indicator
    virtual bool allowsAgentColocation() const { return false; }
    virtual bool stopsPeas() const { return true; } //stops pea if before actor to attack not found
};

class ThiefBotFactory : public Actor {
public:
    ThiefBotFactory(StudentWorld*sw, int startX, int startY, int meanOrReg) : Actor(sw, IID_ROBOT_FACTORY, startX, startY), m_typeOfRobot(meanOrReg) {}
    virtual ~ThiefBotFactory() {}

    virtual void doSomething();
    
    //indicators
    virtual bool stopsPeas() const { return true; } //stops pea if before actor to attack not found
    virtual bool allowsAgentColocation() const { return false; }
private:
    int m_typeOfRobot; //3 is mean and 2 is regular
};

class PickupableItem : public Actor {
public:
    PickupableItem(StudentWorld* sw, int imageID, int startX, int startY) : Actor(sw, imageID, startX, startY) {}
    virtual ~PickupableItem() {}
    
    //indicator
    virtual bool allowsAgentColocation() const { return true; };
    virtual bool blocksBotRange() const { return false; } //to shoot an avatar (ends range)
};

class Crystal : public PickupableItem {
public:
    Crystal(StudentWorld* sw, int startX, int startY) : PickupableItem(sw, IID_CRYSTAL, startX, startY) {}
    virtual ~Crystal() {}
    
    virtual void doSomething();
};

class Goodie : public PickupableItem {
public:
    Goodie(StudentWorld* sw, int imageID, int startX, int startY) : PickupableItem(sw, imageID, startX, startY) {}
    virtual ~Goodie() {}

    //virtual function implementations
    virtual void doSomething();
    virtual void giveGoodie() = 0;
    virtual void givePoints() = 0;
    
    //indicator
    virtual bool isStealable() const { return true; }
};

class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(StudentWorld* sw, int startX, int startY) : Goodie(sw, IID_EXTRA_LIFE, startX, startY) {}
    virtual ~ExtraLifeGoodie() {}
    
    //indicator
    virtual bool givesLife() const { return true; }

    //virtual function implementations
    virtual void giveGoodie();
    virtual void givePoints();
};

class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(StudentWorld* sw, int startX, int startY) : Goodie(sw, IID_RESTORE_HEALTH, startX, startY) {}
    virtual ~RestoreHealthGoodie() {}
    
    //indicator
    virtual bool givesHealth() const { return true; }
    
    
    

    virtual void giveGoodie();
    virtual void givePoints();
};

class AmmoGoodie : public Goodie {
public:
    AmmoGoodie(StudentWorld* sw, int startX, int startY) : Goodie(sw, IID_AMMO, startX, startY) {}
    virtual ~AmmoGoodie() {}
    
    //indicator
    virtual bool givesAmmo() const { return true; }

    

    virtual void giveGoodie();
    virtual void givePoints();
};

class Exit : public Actor {
public:
    Exit(StudentWorld* sw, int startX, int startY) : Actor(sw, IID_EXIT, startX, startY), m_currentVisibility(false) {}
    virtual ~Exit() {}
    
    virtual void doSomething();

    //indicator
    virtual bool allowsAgentColocation() const { return true; }
    
    
    
    bool getCurrentVisibility() { return m_currentVisibility; }
    void setCurrentVisibility(bool m) { m_currentVisibility = m; }
private:
    bool m_currentVisibility;
};

class Marble : public Actor {
public:
    Marble(StudentWorld* sw, int startX, int startY) : Actor(sw, IID_MARBLE, startX, startY) {
        setHealth(10);
    }
    virtual ~Marble() {}
    
    virtual void doSomething();

    //indicator
    virtual bool allowsAgentColocation() const { return false; }
    virtual bool isDamageable() const { return true; } //if a pea colocates, will it be damaged?
    virtual bool isSwallowable() const { return true; } //only marbles (swallowable by pits)
};

class Pit : public Actor {
public:
    Pit(StudentWorld* sw, int startX, int startY) : Actor(sw, IID_PIT, startX, startY) {}
    virtual ~Pit() {}
    
    virtual void doSomething() { return; }
    
    //indicator
    virtual bool allowsAgentColocation() const { return false; }
    virtual bool blocksBotRange() const { return false; } //to shoot an avatar (ends range)
    virtual bool allowsMarble() const { return true; }
    virtual bool allowsMarbleColocation() const { return true; }
};//$

#endif // ACTOR_H_

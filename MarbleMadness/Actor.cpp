#include "Actor.h"
#include "StudentWorld.h"

void Avatar::loseHealth() {
    Actor::loseHealth();
    if (getHealth()<=0) {
        getWorld()->playSound(SOUND_PLAYER_DIE);
        setDead();
        return;
    }
    getWorld()->playSound(SOUND_PLAYER_IMPACT);
}//$

void RegularThiefBot::loseHealth() {
    Actor::loseHealth();
    if (getHealth()<=0) {
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(10);
        
        if (getStealingStatus()) {
            getWorld()->returnAGoodie(getX(),getY(),getGoodieType());
        }
        setDead();
        return;
    }
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
}//$

void MeanThiefBot::loseHealth() {
    Actor::loseHealth();
    if (getHealth()<=0) {
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(20);
        
        if (getStealingStatus()) {
            getWorld()->returnAGoodie(getX(),getY(),getGoodieType());
        }
        setDead();
        return;
    }
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
}//$

void RageBot::loseHealth() {
    Actor::loseHealth();
    if (getHealth()<=0) {
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(100);
        setDead();
        return;
    }
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
}//$

void Agent::pushPea(int dir) { getWorld()->addPea(getX(),getY(),dir); }//$

void Pea::doSomething() {
    if (!isAlive()) { return; }
    //when there is something damageable in its initial position
    if (willItDie()) {
        setDead();
        return;
    }
    
    int newX = getX();
    int newY = getY();
    switch (getDirection()) {
        case right:
            newX++;
            break;
        case left:
            newX--;
            break;
        case up:
            newY++;
            break;
        case down:
            newY--;
            break;
    }
    moveTo(newX,newY);
    if (firstSpot()) {
        if (getWorld()->peaDoesDamage(getX(),getY())) {
            goingToDieSoon();
            return;
        }
        if (getWorld()->blocksPea(getX(),getY())) {
            goingToDieSoon();
            return;
        }
        setFirstSpot();
        return;
    }
    if (getWorld()->peaDoesDamage(newX,newY)) {
        setDead();
        return;
    }
    if (getWorld()->blocksPea(newX,newY)) {
        setDead();
        return;
    }
    switch (getDirection()) {
        case right:
            newX++;
            break;
        case left:
            newX--;
            break;
        case up:
            newY++;
            break;
        case down:
            newY--;
            break;
    }
    if (getWorld()->peaDoesDamage(newX,newY)) {
        goingToDieSoon();
        return;
    }
    if (getWorld()->blocksPea(newX,newY)) {
        goingToDieSoon();
        return;
    }
}//$$$$$$$$$%%%$$$$$SHORTEN$$$$$$$$$$$$$%%%%%$$$$$

void Avatar::doSomething() {
    if (!isAlive()) { return; }
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_ESCAPE:
                getWorld()->playSound(SOUND_PLAYER_DIE);
                setDead();
                break;
            case KEY_PRESS_SPACE:
                if (getPeas()>0) {
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    decPea();
                    pushPea(getDirection());
                }
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                move(left);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                move(right);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                move(up);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                move(down);
                break;
        }
    }
    if (getHealth()<=0) {
        setDead();
        return;
    }
}//$

void Agent::getNewPos(int dir, int& newX, int& newY) {
    int tempX = getX();
    int tempY = getY();
    switch (dir) {
        case left:
            tempX -= 1;
            break;
        case right:
            tempX +=1;
            break;
        case up:
            tempY += 1;
            break;
        case down:
            tempY -=1;
            break;
    }
    newX = tempX;
    newY = tempY;
}//$

bool Agent::canMove(int dir) {
    int tempX, tempY;
    getNewPos(dir, tempX, tempY);
    return getWorld()->isValidPos(tempX,tempY,this);
}//$

bool Avatar::checkPosForMarble(int dir) {
    int tempX, tempY;
    getNewPos(dir,tempX,tempY);
    if (getWorld()->isMarble(tempX,tempY)) { return true; }
    return false;
}//$

bool Agent::move(int dir) {
    int tempX, tempY;
    if (canMove(dir)) {
        getNewPos(dir, tempX, tempY);
        moveTo(tempX,tempY);
        return true;
    }
    if (checkPosForMarble(dir)) {
        getNewPos(dir,tempX,tempY);
        if (getWorld()->moveMarble(tempX,tempY,dir)) {
            moveTo(tempX,tempY);
            return true;
        }
    }
    return false;
}//$

bool Robot::timeToAct() {
    if ( getTicks() >= (28 - getWorld()->getLevel()) / 4 ) {
        resetTicks();
        return true;
    }
    addTicks();
    return false;
}//$

bool Robot::shouldIShoot() {
    return (getWorld()->foundTargetInRange(getX(),getY(),getDirection()));
}//$

void Robot::changeDirection(int dir) {
    if (dir==180 || dir==1) {
        setDirection(right);
    }
    else if (dir==0 || dir==2) {
        setDirection(left);
    }
    else if (dir==90 || dir==3) {
        setDirection(down);
    }
    else if (dir==270 || dir==4) {
        setDirection(up);
    }
}//$

void RageBot::doSomething() {
    if (!isAlive()) { return; }
    if (timeToAct()) {
        if (shouldIShoot()) {
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            pushPea(getDirection());
            return;
        }
        if (!move(getDirection())) { changeDirection(getDirection()); }
    }
    if (getHealth()<=0) {
        setDead();
        return;
    }
}//$

void ThiefBot::doSomething() {
    if (!isAlive()) { return; }
    if (timeToAct()) {
        if (shouldIShoot()) {
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            pushPea(getDirection());
            return;
        }
        int goodieType;
        if (getWorld()->isGoodie(getX(),getY(),goodieType)) {
            setGoodieType(goodieType);
            if ((!getStealingStatus()) && ((rand()%10)==0)) {
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
                stoleAGoodie();
                getWorld()->getActorAt(getX(),getY())->setStolen();
                return;
            }
        }
        if (getDistanceTicker()!=getDistanceBeforeTurning()) {
            if (!move(getDirection())) {
                changeDirection(rand()%4 + 1);
                setNewDistanceBeforeTurning();
                resetDistanceTicker();
                return;
            }
            incDistanceTicker();
            return;
        }
        resetDistanceTicker();
    }
    if (getHealth()<=0) {
        setDead();
    }
    return;
}//$

void AmmoGoodie::giveGoodie() {
    getWorld()->giveAmmo();
}
void AmmoGoodie::givePoints() {
    getWorld()->increaseScore(100);
}

void RestoreHealthGoodie::giveGoodie() {
    getWorld()->giveHealth();
}
void RestoreHealthGoodie::givePoints() {
    getWorld()->increaseScore(500);
}

void ExtraLifeGoodie::giveGoodie() {
    getWorld()->incLives();
}//$
void ExtraLifeGoodie::givePoints() {
    getWorld()->increaseScore(1000);
}//$

void ThiefBotFactory::doSomething() {
    if (!isAlive()) { return; }
    if (getWorld()->surroundingCount(getX(),getY())<3 && !(getWorld()->isThiefBotAt(getX(),getY()))) {
        if (rand()%50 == 0) {
            getWorld()->playSound(SOUND_ROBOT_BORN);
            if (m_typeOfRobot==2) { //regular thiefbot
                getWorld()->addRegularThiefBot(getX(),getY());
            }
            if (m_typeOfRobot==3) { //mean thiefbot
                getWorld()->addMeanThiefBot(getX(),getY());
            }
        }
    }
}

void Marble::doSomething() {
    if (!isAlive()) { return; }
    if (getHealth()<=0) {
        setDead();
        return;
    }
    if (getWorld()->isPit(getX(),getY())) {
        getWorld()->killPit(getX(),getY());
        setDead();
        return;
    }
}

void Exit::doSomething() {
    if (!isAlive()) { return; }
    setVisible(false);
    if ( getWorld()->foundAllCrystals() ) {
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        setCurrentVisibility(true);
        setVisible(true);
    }
    if (getWorld()->playerIsStandingOn(this) && getCurrentVisibility()) {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->setLevelCompletion(true);
        setDead();
        return;
    }
}

void Crystal::doSomething() {
    if (!isAlive()) { return; }
    if (getWorld()->playerIsStandingOn(this)) {
        getWorld()->increaseScore(50);
        getWorld()->foundACrystal();
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        return;
    }
}

void Goodie::doSomething() {
    if (!isAlive()) { return; }
    if (getWorld()->playerIsStandingOn(this) && !isItStolen()) {
        givePoints();
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        giveGoodie();
        return;
    }
    if (isItStolen()) {
        setDead();
        return;
    }
}


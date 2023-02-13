#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor: public GraphObject
{
public:
    Actor(StudentWorld& studentWorld, int imageID, int startX, int startY, int dir, int depth, double size);
    virtual void doSomething() = 0;
    virtual bool isSolid() {return false;};
    bool isAlive() {return m_alive;};
    void setAlive(bool b) {m_alive = b;};
    virtual void bonk(){};
    StudentWorld& world() {return m_studentWorld;};
    virtual bool isDamageable() {return true;};
    bool getDamaged() {return damaged;};
    void setDamaged(bool b) {damaged = b;};
    virtual bool isEnemy() {return false;};
    virtual void inflictDamage() {};
private:
    bool m_alive = true;
    StudentWorld& m_studentWorld;
    bool damaged = false;
    
};

class Blipe : public Actor
{
public:
    Blipe(StudentWorld& studentWorld, int ID, int x, int y);
    void doSomething() {};
    bool isSolid() {return true;};
    virtual void bonk() {};
    bool isDamageable() {return false;};
};

class Block : public Blipe
{
public:
    Block(StudentWorld& studentWorld, int startX, int startY);
    void bonk();
    virtual void spawnPowerUp() {};
    void setSpawned(bool b) {spawned = b;};
    bool getSpawned() {return spawned;};
private:
    bool spawned = false;
};

class FlowerBlock : public Block
{
public:
    FlowerBlock(StudentWorld& studentWorld, int startX, int startY);
    void spawnPowerUp();
};

class MushroomBlock : public Block
{
public:
    MushroomBlock(StudentWorld& studentWorld, int startX, int startY);
    void spawnPowerUp();
};

class StarBlock : public Block
{
public:
    StarBlock(StudentWorld& studentWorld, int startX, int startY);
    void spawnPowerUp();
};

class Pipe : public Blipe
{
public:
    Pipe(StudentWorld& studentWorld, int startX, int startY);
};

class Peach : public Actor
{
public:
    Peach(StudentWorld& studentWorld, int startX, int startY);
    void doSomething();
    void bonk();
    void inflictDamage();
    int getHitPoint() {return m_hitPoint;};
    void setHitPoint(int x) {m_hitPoint = x;};
    bool getShootPow() {return shootPow;};
    void setShootPow(bool b) {shootPow = b;};
    bool getJumpPow() {return jumpPow;};
    void setJumpPow(bool b) {jumpPow = b;};
    bool getStarPow() {return starPow;};
    void setStarPow() {starPow = true;};
    void setStarTime() {m_starTime = 150;};
    bool getTempInvincible() {return tempInvinc;};
    
private:
    int m_hitPoint = 1;
    bool power = false;
    int m_starTime = 0;
    bool starPow = false;
    int m_jumpDis = 0;
    bool jumpPow = false;
    bool shootPow = false;
    int m_shootRecharge = 0;
    bool tempInvinc = false;//PROBABLY SHOULD IMPLEMENT THIS
    int m_timeInvinc = 0;
};

class Projectile : public Actor
{
public:
    Projectile(StudentWorld& studentWorld, int ID, int x, int y, int dir);
    void doSomething();
    virtual bool canDamagePeach() = 0;
    bool isDamageable() {return false;};
private:
    
};

class PiranhaFireball : public Projectile
{
public:
    PiranhaFireball(StudentWorld& studentWorld, int x, int y, int dir);
    bool canDamagePeach() {return true;};
};

class PeachFireball : public Projectile
{
public:
    PeachFireball(StudentWorld& studentWorld, int x, int y, int dir);
    PeachFireball(StudentWorld& studentWorld, int ID, int x, int y, int dir);
    bool canDamagePeach() {return false;};
private:
};

class Shell : public PeachFireball
{
public:
    Shell(StudentWorld& studentWorld, int x, int y, int dir);
};

class Flario : public Actor
{
public:
    Flario(StudentWorld& studentWorld, int ID, int x, int y);
    bool isDamageable() {return false;};
    virtual int returnStatus() = 0;
    void doSomething();
};

class Flag : public Flario
{
public:
    Flag(StudentWorld& studentWorld, int x, int y);
    int returnStatus() {return GWSTATUS_FINISHED_LEVEL;};
};

class Mario : public Flario
{
public:
    Mario(StudentWorld& studentWorld, int x, int y);
    int returnStatus() {return GWSTATUS_PLAYER_WON;};
};

class Goodies : public Actor
{
public:
    Goodies(StudentWorld& studentWorld, int ID, int x, int y);
    void doSomething();
    virtual int pointInc() = 0;
    bool isDamageable() {return false;};
};

class Flower : public Goodies
{
public:
    Flower(StudentWorld& studentWorld, int x, int  y);
    int pointInc() {return 50;};
};

class Mushroom : public Goodies
{
public:
    Mushroom(StudentWorld& studentWorld, int x, int y);
    int pointInc() {return 75;};
};

class Star : public Goodies
{
public:
    Star(StudentWorld& studentWorld, int x, int y);
    int pointInc() {return 100;};
};

class Enemy : public Actor
{
public:
    Enemy(StudentWorld& studentWorld, int ID, int x, int y);
    void doSomething();
    void bonk();
    virtual bool canMakeShell() {return false;};
    void inflictDamage();
    bool isEnemy() {return true;};
};

class Goomba : public Enemy
{
public:
    Goomba(StudentWorld& studentWorld, int x, int y);
};

class Koopa : public Enemy
{
public:
    Koopa(StudentWorld& studentWorld, int x, int y);
    bool canMakeShell() {return true;};
};

class Piranha : public Enemy
{
public:
    Piranha(StudentWorld& studentWorld, int x, int y);
    void doSomething();
private:
    int m_shootDelay = 0;
};
#endif // ACTOR_H_

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cstdlib>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld& studentWorld, int imageID, int startX, int startY, int dir, int depth, double size)
    : m_studentWorld(studentWorld), GraphObject(imageID, startX, startY, dir, depth, size)
{}

Blipe::Blipe(StudentWorld& studentWorld, int ID, int x, int y)
    :Actor(studentWorld, ID, x, y, 0, 2, 1.0)
{}

Block::Block(StudentWorld& studentWorld, int startX, int startY)
    : Blipe(studentWorld, IID_BLOCK, startX, startY)
{}

FlowerBlock::FlowerBlock(StudentWorld& studentWorld, int startX, int startY)
    : Block(studentWorld, startX, startY)
{}

void FlowerBlock::spawnPowerUp()
{
    world().addActor(new Flower(world(), getX(), getY()+8));
    setSpawned(true);
}

MushroomBlock::MushroomBlock(StudentWorld& studentWorld, int startX, int startY)
    : Block(studentWorld, startX, startY)
{}

void MushroomBlock::spawnPowerUp()
{
    world().addActor(new Mushroom(world(), getX(), getY()+8));
    setSpawned(true);
}

StarBlock::StarBlock(StudentWorld& studentWorld, int startX, int startY)
    : Block(studentWorld, startX, startY)
{}

void StarBlock::spawnPowerUp()
{
    world().addActor(new Star(world(), getX(), getY()+8));
    setSpawned(true);
    world().peachPtr()->setStarTime();
}

void Block::bonk()
{
    cerr << "bonk was accessed!" << endl;
    world().playSound(SOUND_PLAYER_BONK);
    if(!getSpawned())
        spawnPowerUp();
}

Pipe::Pipe(StudentWorld& studentWorld, int startX, int startY)
    : Blipe(studentWorld, IID_PIPE, startX, startY)
{}

Peach::Peach(StudentWorld& studentWorld, int startX, int startY)
    :Actor(studentWorld, IID_PEACH, startX, startY, 0, 0, 1.0)
{}

void Peach::doSomething()
{
    if (!isAlive())
        return;
    
    if (starPow)
    {
        m_starTime--;
        if(m_starTime == 0)
            starPow = false;
    }
    
    else if (tempInvinc)
    {
        m_timeInvinc--;
        if (m_timeInvinc == 0)
            tempInvinc = false;
    }
    
    if (m_shootRecharge > 0)
        m_shootRecharge--;
    
    StudentWorld &tempWorld = world();
    int x = getX();
    int y = getY();
    
    vector<Actor*>  colliding = tempWorld.actorsOverlaping(x, y, this);
    if(colliding.size() != 0)
    {
        colliding.at(0)->bonk();
        m_jumpDis = 0;
    }
    
    if (m_jumpDis > 0)
    {
        vector<Actor*>  col = tempWorld.actorsOverlaping(x, y+4, this);
        if(col.size() != 0 && col.at(0)->isSolid())
        {
            col.at(0)->bonk();
            m_jumpDis = 0;
        }
        else
        {
            moveTo(x, y+4);
            m_jumpDis--;
        }
    }
    else
    {
        vector<Actor*> c = tempWorld.actorsOverlaping(x, y-4, this);
        bool hasSolid = false;
        for(int i = 0; i < c.size(); i++)
        {
            if(c.at(i)->isSolid())
                hasSolid = true;
        }
        if (c.size() == 0 || !hasSolid)
            moveTo(x, y-4);
    }
    
    int dir;
    if(tempWorld.getKey(dir))
    {
        if(dir == KEY_PRESS_LEFT)
        {
            setDirection(180);
            
            vector<Actor*>  col = tempWorld.actorsOverlaping(x-4, y, this);
            if(col.size() != 0 && col.at(0)->isSolid())
                col.at(0)->bonk();
            else
                moveTo(x-4, y);
        }
        else if(dir == KEY_PRESS_RIGHT)
        {
            setDirection(0);
            
            vector<Actor*>  col = tempWorld.actorsOverlaping(x+4, y, this);
            if(col.size() != 0 && col.at(0)->isSolid())
                col.at(0)->bonk();
            /*if(tempWorld.hasOverlap(x+4, y, this))
            {
                Actor* tempAct = tempWorld.findActorAt(x+8, y);
                if(tempAct != nullptr)//DO I NEED THIS CHECK???????
                    tempAct->bonk();
            }*/
            else
                moveTo(x+4, y);
        }
        else if(dir == KEY_PRESS_UP)
        {
            if(tempWorld.hasOverlap(x, y-8, this))
            {
                if (jumpPow)
                    m_jumpDis = 12;
                else
                    m_jumpDis = 8;
                world().playSound(SOUND_PLAYER_JUMP);
            }
        }
        else if(dir == KEY_PRESS_SPACE)
        {
            if(shootPow)
            {
                if (m_shootRecharge <= 0)
                {
                    world().playSound(SOUND_PLAYER_FIRE);
                    m_shootRecharge = 8;
                    
                    int peachDir = getDirection();
                    if (peachDir == 0)
                    {
                        world().addActor(new PeachFireball(world(), x+12, y, 0));//check these coords
                    }
                    else
                    {
                        world().addActor(new PeachFireball(world(), x-4, y, 180));//check these coords
                    }
                }
            }
        }
    }
}

void Peach::bonk()
{
    if (getStarPow() || getTempInvincible())
        return;
    m_hitPoint--;
    m_timeInvinc = 10;
    setShootPow(false);
    setJumpPow(false);
    if(m_hitPoint >= 1)
        world().playSound(SOUND_PLAYER_HURT);
    else
    {
        StudentWorld& tempWorld = world();
        if(tempWorld.peachPtr()->isAlive())
            tempWorld.decLives();
        setAlive(false);
        tempWorld.setStatus(GWSTATUS_PLAYER_DIED);
        cerr << "it's working" << endl;
        cerr << "number of lives: " << tempWorld.getLives() << endl;
    }
}

void Peach::inflictDamage()
{
    bonk();
}

Projectile::Projectile(StudentWorld& studentWorld, int ID, int x, int y, int dir)
    : Actor(studentWorld, ID, x, y, dir, 1, 1.0)
{}

void Projectile::doSomething()
{
    int x = getX();
    int y = getY();
    StudentWorld& tempWorld = world();
    vector<Actor*> vAct = tempWorld.actorsOverlaping(x, y, this);
    Actor* tempAct;
    if(vAct.size() != 0)
        tempAct = vAct.at(0);
    if(canDamagePeach())
    {
        if (vAct.size() != 0 && tempAct == tempWorld.peachPtr() && tempAct->isAlive())
        {
            tempAct->inflictDamage();
            setAlive(false);
            return;
        }
    }
    else
    {
        if(vAct.size() != 0 && tempAct != tempWorld.peachPtr() && tempAct->isAlive() && tempAct->isDamageable())
        {
            cerr << "shell is killing something" << endl;
            tempAct->inflictDamage();
            setAlive(false);
            return;
        }
    }
    vector<Actor*>  collide = tempWorld.actorsOverlaping(x, y-2, this);
    if (collide.size() == 0 || collide.at(0)->isEnemy())
    {
        y-=2;
        moveTo(x, y);
    }
    if(getDirection() == 0)
    {
        vector<Actor*>  col = tempWorld.actorsOverlaping(x+2, y, this);
        if(col.size() != 0 && col.at(0)->isSolid())
        {
            setAlive(false);
            return;
        }
        
        else
            moveTo(x+2, y);
    }
    if(getDirection() == 180)
    {
        vector<Actor*>  col = tempWorld.actorsOverlaping(x-2, y, this);
        if(col.size() != 0 && col.at(0)->isSolid())
        {
            setAlive(false);
            return;
        }
        else
            moveTo(x-2, y);
    }
}

PiranhaFireball::PiranhaFireball(StudentWorld& studentWorld, int x, int y, int dir)
    : Projectile(studentWorld, IID_PIRANHA_FIRE, x, y, dir)
{}

PeachFireball::PeachFireball(StudentWorld& studentWorld, int x, int y, int dir)
    :Projectile(studentWorld, IID_PEACH_FIRE, x, y, dir)
{}

PeachFireball::PeachFireball(StudentWorld& studentWorld, int ID, int x, int y, int dir)
    :Projectile(studentWorld, ID, x, y, dir)
{}

Shell::Shell(StudentWorld& studentWorld, int x, int y, int dir)
    :PeachFireball(studentWorld, IID_SHELL, x, y, dir)
{}

Flario::Flario(StudentWorld& studentWorld, int ID, int x, int y)
    :Actor(studentWorld, ID, x, y, 0, 1, 1)
{}

void Flario::doSomething()
{
    if (!isAlive())
        return;
    StudentWorld& tempWorld = world();
    int x = getX();
    int y = getY();
    if(tempWorld.overlapsPeach(x, y, this))
    {
        tempWorld.increaseScore(1000);
        setAlive(false);
        tempWorld.setStatus(returnStatus());
    }
}

Flag::Flag(StudentWorld& studentWorld, int x, int y)
    : Flario(studentWorld, IID_FLAG, x, y)
{}

Mario::Mario(StudentWorld& studentWorld, int x, int y)
    : Flario(studentWorld, IID_MARIO, x, y)
{}

Goodies::Goodies(StudentWorld& studentWorld, int ID, int x, int y)
    : Actor(studentWorld, ID, x, y, 0, 1, 1)
{}

void Goodies::doSomething()
{
    StudentWorld& tempWorld = world();
    int x = getX();
    int y = getY();
    if(tempWorld.overlapsPeach(x, y, this))
    {
        int pts = pointInc();
        tempWorld.increaseScore(pts);
        Peach* p = tempWorld.peachPtr();
        if(pts == 50)
        {
            p->setShootPow(true);
            p->setHitPoint(2);
        }
        if(pts == 75)
        {
            p->setJumpPow(true);
            p->setHitPoint(2);
        }
        if(pts == 100)
            p->setStarPow();
        setAlive(false);
        tempWorld.playSound(SOUND_PLAYER_POWERUP);
        return;
    }
    vector<Actor*> aV = tempWorld.actorsOverlaping(x, y-2, this);
    bool hasSolid = false;
    for(int i = 0; i < aV.size(); i++)
    {
        if(aV.at(i)->isSolid())
            hasSolid = true;
    }
    if (tempWorld.overlapsPeach(x, y-2, this) || !hasSolid)
    {
        y-=2;
        moveTo(x, y);
    }
    if (getDirection() == 0)
    {
        vector<Actor*> aV1 = tempWorld.actorsOverlaping(x+2, y, this);
        bool hasSolid1 = false;
        for(int i = 0; i < aV1.size(); i++)
        {
            if(aV1.at(i)->isSolid())
                hasSolid1 = true;
        }
        if(!(tempWorld.overlapsPeach(x+2, y, this)) && hasSolid1)
        {
            setDirection(180);
            return;
        }
        else
            moveTo(x+2, y);
    }
    if (getDirection() == 180)
    {
        vector<Actor*> aV2 = tempWorld.actorsOverlaping(x-2, y, this);
        bool hasSolid2 = false;
        for(int i = 0; i < aV2.size(); i++)
        {
            if(aV2.at(i)->isSolid())
                hasSolid2
                = true;
        }
        if(!(tempWorld.overlapsPeach(x-2, y, this)) && hasSolid2)
        {
            setDirection(0);
            return;
        }
        else
            moveTo(x-2, y);
    }
    
}

Flower::Flower(StudentWorld& studentWorld, int x, int y)
    : Goodies(studentWorld, IID_FLOWER, x, y)
{}

Mushroom::Mushroom(StudentWorld& studentWorld, int x, int y)
    :Goodies(studentWorld, IID_MUSHROOM, x, y)
{}

Star::Star(StudentWorld& studentWorld, int x, int y)
    :Goodies(studentWorld, IID_STAR, x, y)
{}

Enemy::Enemy(StudentWorld& studentWorld, int ID, int x, int y)
    :Actor(studentWorld, ID, x, y, randInt(0, 1) * 180, 0, 1)
{}

void Enemy::doSomething()
{
    if(!isAlive())
        return;
    
    StudentWorld& tempWorld = world();
    int x = getX();
    int y = getY();
    
    if(tempWorld.overlapsPeach(x, y, this))
    {
        tempWorld.peachPtr()->bonk();
        return;
    }

    if (getDirection() == 0)
    {
        vector<Actor*> col = tempWorld.actorsOverlaping(x+1, y, this);
        Actor* t;
        if (col.size() > 0)
            t = col.at(0);
        if(col.size() != 0 && t->isSolid() && t != tempWorld.peachPtr())
        {
            //&& t != tempWorld.peachPtr()
            setDirection(180);
        }
        else
        {
            vector<Actor*>  colliding = tempWorld.actorsOverlaping(x+SPRITE_WIDTH, y-1, this);
            if(colliding.size() != 0)
            {
                //&& colliding.at(0)->isSolid()
                // && colliding.at(0) != tempWorld.peachPtr()
                bool hasSolid = false;
                for (int i = 0; i < colliding.size(); i++)
                {
                    if(colliding.at(i)->isSolid())
                        hasSolid = true;
                }
                /*int otherY = -1;
                if(tempWorld.overlapsPeach(x-SPRITE_WIDTH, y-1, this))
                   otherY = tempWorld.peachPtr()->getY();*/
                if(!hasSolid)
                    setDirection(180);
                else
                //{
                    //int otherX = col.at(0)->getX();
                    moveTo(x+1, y);
                //}
            }
            else
                //if(col.size() != 0 && col.at(0) == tempWorld.peachPtr())
                    setDirection(180);
        }
    }
    
    else
    {
        vector<Actor*>  col = tempWorld.actorsOverlaping(x-1, y, this);
        Actor* t;
        if (col.size() > 0)
            t = col.at(0);
        if(col.size() != 0 && t->isSolid() && t != tempWorld.peachPtr())
        {
            //&& t != tempWorld.peachPtr()
            setDirection(0);
        }
        else
        {
            vector<Actor*>  colliding = tempWorld.actorsOverlaping(x-SPRITE_WIDTH, y-1, this);
            if(colliding.size() != 0)
            {
                // && colliding.at(0) != tempWorld.peachPtr()
                bool hasSolid = false;
                for (int i = 0; i < colliding.size(); i++)
                {
                    if(colliding.at(i)->isSolid())
                        hasSolid = true;
                }
                /*int otherY = -1;
                if(tempWorld.overlapsPeach(x-SPRITE_WIDTH, y-1, this))
                    otherY = tempWorld.peachPtr()->getY();
                if(col.size() != 0 && y - otherY == 8)//LAST CHANGE
                    return;*/
                if(!hasSolid)
                    setDirection(0);
                else
               // {
                    moveTo(x-1, y);
                //}
            }
            else
                //if(col.size() != 0 && col.at(0) == tempWorld.peachPtr())
                    setDirection(0);
        }
    }
    
}

void Enemy::bonk()
{
    StudentWorld& tempWorld = world();
    int x = getX();
    int y = getY();
    if(!tempWorld.overlapsPeach(x, y, this))
        return;
    
    Peach* p = tempWorld.peachPtr();
    if(p->getStarPow())
    {
        tempWorld.playSound(SOUND_PLAYER_KICK);
        tempWorld.increaseScore(100);
        setAlive(false);
        if(canMakeShell())
            tempWorld.addActor(new Shell(tempWorld, x, y, getDirection()));
    }
    
}

void Enemy::inflictDamage()
{
    StudentWorld& tempWorld = world();
    tempWorld.increaseScore(100);
    setAlive(false);
    if(canMakeShell())
    {
        tempWorld.addActor(new Shell(tempWorld, getX(), getY(), getDirection()));
        cerr << "made a new shell" << endl;
    }
    setDamaged(true);
}

Goomba::Goomba(StudentWorld& studentWorld, int x, int y)
    :Enemy(studentWorld, IID_GOOMBA, x, y)
{}

Koopa::Koopa(StudentWorld& studentWorld, int x, int y)
    :Enemy(studentWorld, IID_KOOPA, x, y)
{}

Piranha::Piranha(StudentWorld& studentWorld, int x, int y)
    :Enemy(studentWorld, IID_PIRANHA, x, y)
{}

void Piranha::doSomething()
{
    if(!isAlive())
        return;
    
    increaseAnimationNumber();
    
    StudentWorld& tempWorld = world();
    int x = getX();
    int y = getY();
    
    if(tempWorld.overlapsPeach(x, y, this))
    {
        tempWorld.peachPtr()->bonk();
        return;
    }
    
    if(tempWorld.peachYOverlap(y + (1.5*SPRITE_HEIGHT))
        || tempWorld.peachYOverlap(y - (1.5*SPRITE_HEIGHT)))
    {
        int peachX = tempWorld.peachPtr()->getX();
        if (x > peachX)
            setDirection(180);
        else
            setDirection(0);
        if(m_shootDelay > 0)
        {
            m_shootDelay--;
            return;
        }
        else
        {
            if(getDirection() == 0)
            {
                if(peachX - x < 8*SPRITE_WIDTH)
                {
                    tempWorld.addActor(new PiranhaFireball(tempWorld, x, y, 0));
                    tempWorld.playSound(SOUND_PIRANHA_FIRE);
                    m_shootDelay = 40;
                }
            }
            else
            {
                if(x - peachX < 8*SPRITE_WIDTH)
                {
                    tempWorld.addActor(new PiranhaFireball(tempWorld, x, y, 180));
                    tempWorld.playSound(SOUND_PIRANHA_FIRE);
                    m_shootDelay = 40;
                }
            }
        }
    }
    else
    {
        return;
    }
}

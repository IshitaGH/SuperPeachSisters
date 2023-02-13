#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <sstream>
#include "Level.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    string levelText = "level--.txt";
    int level = getLevel();
    levelText.at(5) = '0' + level/10;
    levelText.at(6) = '0' + level%10;
    
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(levelText);
    
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        Level::GridEntry ge;
        for (int i = 0; i < GRID_WIDTH; i++)
        {
            for(int j = 0; j < GRID_HEIGHT; j++)
            {
                ge = lev.getContentsOf(i, j);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::koopa:
                        m_actors.push_back(new Koopa(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::goomba:
                        m_actors.push_back(new Goomba(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::peach:
                        peach = new Peach(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT);
                        m_actors.push_back(peach);
                        break;
                    case Level::flag:
                        m_actors.push_back(new Flag(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::block:
                        m_actors.push_back(new Block(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::star_goodie_block:
                        m_actors.push_back(new StarBlock(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::piranha:
                        m_actors.push_back(new Piranha(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::mushroom_goodie_block:
                        m_actors.push_back(new MushroomBlock(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::flower_goodie_block:
                        m_actors.push_back(new FlowerBlock(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::pipe:
                        m_actors.push_back(new Pipe(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::mario:
                        m_actors.push_back(new Mario(*this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                }
            }
        }
        
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    status = GWSTATUS_CONTINUE_GAME;
    for (int i = 0; i < m_actors.size(); i++)
    {
        m_actors.at(i)->doSomething();
    }
    
    int i = 0;
    while (i < m_actors.size())
    {
        Actor* tempAct = m_actors.at(i);
        if (!(tempAct->isAlive()))
        {
            vector<Actor*>::iterator vi = m_actors.begin();
            vi +=i;
            m_actors.erase(vi);
            delete tempAct;
        }
        else
            i++;
    }
    
    ostringstream statusText;
    statusText << "Lives: " << getLives() << "  Level: " << getLevel() << "  Points: " << getScore();
    
    if(peach->getStarPow())
        statusText << " StarPower!";
    if(peach->getShootPow())
        statusText << " ShootPower!";
    if(peach->getJumpPow())
        statusText << " JumpPower!";
    
    string displayText = statusText.str();
    setGameStatText(displayText);
    
    
    return status;
}

void StudentWorld::cleanUp()
{
    while (m_actors.size() != 0)
    {
        delete m_actors.back();
        m_actors.pop_back();
    }

}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

bool StudentWorld::hasOverlap(int startX, int startY, Actor* act)
{
    int startXEnd = startX + SPRITE_WIDTH;
    int startYEnd = startY + SPRITE_HEIGHT;
    
    int vSize = m_actors.size();
    for(int i = 0; i < vSize; i++)
    {
        Actor* tempAct = m_actors.at(i);
        if (tempAct == act)
            continue;
        int otherX = tempAct->getX();
        int otherXEnd = otherX + SPRITE_WIDTH;
        
        int otherY = tempAct->getY();
        int otherYEnd = otherY + SPRITE_HEIGHT;
        
        
        if (otherYEnd > startY && otherY < startYEnd && otherXEnd > startX && otherX < startXEnd)
            return true;

    }
    
    return false;
}

bool StudentWorld::overlapsPeach(int startX, int startY, Actor* act)
{
    int startXEnd = startX + SPRITE_WIDTH;
    int startYEnd = startY + SPRITE_HEIGHT;
    
    int vSize = m_actors.size();
    for(int i = 0; i < vSize; i++)
    {
        Actor* tempAct = m_actors.at(i);
        if (tempAct == act)
            continue;
        int otherX = tempAct->getX();
        int otherXEnd = otherX + SPRITE_WIDTH;
        
        int otherY = tempAct->getY();
        int otherYEnd = otherY + SPRITE_HEIGHT;
        
        
        if (otherYEnd > startY && otherY < startYEnd && otherXEnd > startX && otherX < startXEnd)
            if(tempAct == peach)
                return true;

    }
    
    return false;
}

vector<Actor*> StudentWorld::actorsOverlaping(int startX, int startY, Actor* act)
{
    vector<Actor*> av;
    int startXEnd = startX + SPRITE_WIDTH;
    int startYEnd = startY + SPRITE_HEIGHT;
    
    int vSize = m_actors.size();
    for(int i = 0; i < vSize; i++)
    {
        Actor* tempAct = m_actors.at(i);
        if (tempAct == act)
            continue;
        int otherX = tempAct->getX();
        int otherXEnd = otherX + SPRITE_WIDTH;
        
        int otherY = tempAct->getY();
        int otherYEnd = otherY + SPRITE_HEIGHT;
        
        
        if (otherYEnd > startY && otherY < startYEnd && otherXEnd > startX && otherX < startXEnd)
            if(tempAct->isAlive())
                av.push_back(tempAct);
    }
    
    return av;
}

void StudentWorld::addActor(Actor* a)
{
    cerr << "added an actor" << endl;
    m_actors.push_back(a);
}

bool StudentWorld::peachYOverlap(int y)
{
    int yEnd = y + SPRITE_HEIGHT;
    int pStart = peach->getY();
    int pEnd = pStart + SPRITE_HEIGHT;
    if(yEnd > pStart && y < pEnd)
        return true;
    else
        return false;
}

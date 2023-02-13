#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include "Actor.h"
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld();
    bool hasOverlap(int startX, int startY, Actor* act);
    bool overlapsPeach(int startX, int startY, Actor* act);
    std::vector<Actor*> actorsOverlaping(int startX, int startY, Actor* act);
    void addActor(Actor* a);
    Peach* peachPtr() {return peach;};
    int getStatus() {return status;};
    void setStatus(int s) {status = s;};
    bool peachYOverlap(int y);

private:
    std::vector <Actor*> m_actors;
    Peach* peach;
    int status;
};

#endif // STUDENTWORLD_H_

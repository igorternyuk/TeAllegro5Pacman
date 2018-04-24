#include "entity.hpp"
#include <iostream>
#include <cmath>
#include <map>
#include <algorithm>

Entity::Entity(int x, int y, Direction dir, charMatrix map,
               ALLEGRO_BITMAP *bitmap) :
    mX(x), mY(y), mInitialX(x), mInitialY(y), mDir(dir), mMap(map),
    mBitmap(bitmap)
{}

int Entity::getX() const
{
    return mX;
}

int Entity::getY() const
{
    return mY;
}

void Entity::setMap(const charMatrix &map)
{
    mMap = map;
}

void Entity::setDirection(Direction dir)
{
    mDir = dir;
}

Direction Entity::getDirection() const
{
    return mDir;
}

const charMatrix &Entity::getMap() const
{
    return mMap;
}

void Entity::move()
{
    int nextX = mX, nextY = mY;
    switch(mDir)
    {
        case Direction::LEFT :
            --nextX;
            break;
        case Direction::RIGHT :
            ++nextX;
            break;
        case Direction::UP :
            --nextY;
            break;
        case Direction::DOWN:
            ++nextY;
            break;
        default:
             break;
    }
    bool isNextPosOK = true;
    if(nextY < 0)
    {
        if(mMap[mY][mX] == 'I')
            nextY = mMap.size() - 1;
        else
            isNextPosOK = false;
    }
    if(nextY > mMap.size() - 1)
    {
        if(mMap[mY][mX] == 'I')
            nextY = 0;
        else
            isNextPosOK = false;
    }
    if(nextX < 0)
    {
        if(mMap[mY][mX] == 'I')
            nextX = mMap[nextY].size() - 1;
        else
            isNextPosOK = false;
    }
    if(nextX > mMap[nextY].size() - 1)
    {
        if(mMap[mY][mX] == 'I')
            nextX = 0;
        else
            isNextPosOK = false;
    }
    if(isNextPosOK && mMap[nextY][nextX] != 'X')
    {
        mX = nextX;
        mY = nextY;
    }
}

bool Entity::isCollision(int entityX, int entityY)
{
    return (mX == entityX && mY == entityY);
}

void Entity::resetPosition()
{
    mX = mInitialX;
    mY = mInitialY;
}

bool Entity::isPossibleDirection(Direction dir)
{
    int nx = mX;
    int ny = mY;
    switch(dir)
    {
         case Direction::LEFT :
             --nx;
             break;
         case Direction::RIGHT :
             ++nx;
             break;
         case Direction::UP :
             --ny;
             break;
         case Direction::DOWN :
             ++ny;
             break;
         default:
             break;
    }
    return ny >= 0 && ny < int(mMap.size()) && nx >= 0 &&
            nx < int(mMap[ny].size()) && (mMap[ny][nx] != 'X');
}

void Entity::chooseShortestWay(int targetX, int targetY)
{
   Direction allDirs[] =  {Direction::LEFT, Direction::RIGHT, Direction::UP,
                           Direction::DOWN };
   std::multimap<float, Direction> distDirMap;
   for(auto dir: allDirs)
   {
       if(isPossibleDirection(dir))
       {
           float dist = caclDistanceToTheTarget(dir, targetX, targetY);
           distDirMap.insert(std::make_pair(dist, dir));
       }
   }

   this->mDir = distDirMap.begin()->second;
}

float Entity::caclDistanceToTheTarget(Direction dir, int targetX, int targetY)
{
    switch (dir) {
        case Direction::LEFT:
            return (mX - 1 - targetX) * (mX - 1 - targetX) +
                    (mY - targetY) * (mY - targetY);
        case Direction::RIGHT:
            return (mX + 1 - targetX) * (mX + 1 - targetX) +
                    (mY - targetY) * (mY - targetY);
        case Direction::UP:
            return (mX - targetX) * (mX - targetX) +
                    (mY - 1 - targetY) * (mY - 1 - targetY);
        case Direction::DOWN:
            return (mX - targetX) * (mX - targetX) +
                    (mY + 1 - targetY) * (mY + 1 - targetY);
        default:
            break;
    }
}

void Entity::chooseRandomDir()
{
   Direction randDir;
   bool isDirOK = true;
   do
   {
       int randNumber = rand() % 1000;
       if(randNumber >= 0 && randNumber < 250)
           randDir = Direction::LEFT;
       else if(randNumber >= 250 && randNumber < 500)
           randDir = Direction::RIGHT;
       else if(randNumber >= 500 && randNumber < 750)
           randDir = Direction::UP;
       else if(randNumber >= 750 && randNumber < 1000)
           randDir = Direction::DOWN;
       isDirOK = isPossibleDirection(randDir);
   } while(!isDirOK);
   this->mDir = randDir;
}

bool Entity::collidesWall()
{
    return !isPossibleDirection(this->mDir);
}

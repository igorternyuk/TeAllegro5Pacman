#include "character.h"

Character::Character(int x, int y, Direction dir, charMatrix map, std::string pathToImage) :
    x_(x), y_(y), dir_(dir), map_(map), INIT_PAC_X(x), INIT_PAC_Y(y)
{
    sprite_ = al_load_bitmap(pathToImage.c_str());
}

Character::~Character()
{
    if(sprite_ != nullptr)
        al_destroy_bitmap(sprite_);
}

int Character::getX() const
{
    return x_;
}

int Character::getY() const
{
    return y_;
}

void Character::changeMap(charMatrix map)
{
    map_ = map;
}

void Character::setDirection(Direction dir)
{
    dir_ = dir;
}

Direction Character::getDirection() const
{
    return dir_;
}

charMatrix Character::getMap() const
{
    return map_;
}

void Character::move()
{
    if(dir_ == STOP)
        return;
    int nextX = x_, nextY = y_;
    switch(dir_)
    {
        case LEFT :
            --nextX;
            break;
        case RIGHT :
            ++nextX;
            break;
        case UP :
            --nextY;
            break;
        case DOWN:
            ++nextY;
            break;
        default:
             break;
    }
    bool isNextPosOK = true;
    if(nextY < 0)
    {
        if(map_[y_][x_] == 'I')
            nextY = map_.size() - 1;
        else
            isNextPosOK = false;
    }
    if(nextY > map_.size() - 1)
    {
        if(map_[y_][x_] == 'I')
            nextY = 0;
        else
            isNextPosOK = false;
    }
    if(nextX < 0)
    {
        if(map_[y_][x_] == 'I')
            nextX = map_[nextY].size() - 1;
        else
            isNextPosOK = false;
    }
    if(nextX > map_[nextY].size() - 1)
    {
        if(map_[y_][x_] == 'I')
            nextX = 0;
        else
            isNextPosOK = false;
    }
    if(isNextPosOK && map_[nextY][nextX] != 'X')
    {
        x_ = nextX;
        y_ = nextY;
    }
}

bool Character::isCollision(int entityX, int entityY)
{
    return (x_ == entityX && y_ == entityY);
}

void Character::resetPosition()
{
    x_ = INIT_PAC_X;
    y_ = INIT_PAC_Y;
}

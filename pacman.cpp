#include "pacman.hpp"
#include "fruit.hpp"
#include <cmath>
#include <cassert>
#include <iostream>

Pacman::Pacman(int x, int y, Direction dir, charMatrix map, int numLives,
               ALLEGRO_BITMAP *bitmap, int sourceX, int sourceY,
               int numPictInAnim) :
    Entity(x, y, dir, map, bitmap),
    INIT_LIVES_NUMBER(numLives), numLives_(numLives), mSourceX(sourceX),
    mSourceY(sourceY), mNumPictInAnim(numPictInAnim)
{}

void Pacman::update()
{

    //Обработка события таймера анимации пакмена
    if(mIsMoving || this->mIsWounded)
    {
        std::cout << "pacman update" << std::endl;
        ++mSourceX;
        if(mSourceX >= mNumPictInAnim)
        {
            mSourceX = 0;
            mSourceY = 0;
            if(mIsWounded)
            {
                resetPosition();
                healWound();
                //al_start_timer(mFantomTimer);
            }
        }
    }
    else
    {
       mSourceX = 0;
    }
}

void Pacman::render(int tileSize)
{
    float pacImAngle = 0.0f;
    int flag = NULL;
    switch(getDirection())
    {
        case Direction::LEFT :
            pacImAngle = 0.0f;
            break;
        case Direction::RIGHT :
            pacImAngle = 180.0f;
            flag = ALLEGRO_FLIP_VERTICAL;
            break;
        case Direction::UP :
            pacImAngle = 90.0f;
            break;
        case Direction::DOWN :
            pacImAngle = 270.0f;
            break;
        default:
            pacImAngle = 0.0f;
            break;
    }
    const float pacmanScale = 1.0f;
    ALLEGRO_COLOR filtredColor = al_map_rgb(255, 255, 255);
    al_draw_tinted_scaled_rotated_bitmap_region(
        mBitmap, mSourceX * tileSize, mSourceY * tileSize, tileSize,
        tileSize, filtredColor, tileSize / 2, tileSize / 2,
        getX() * tileSize + tileSize / 2,
        getY() * tileSize + tileSize / 2, pacmanScale, pacmanScale,
        pacImAngle * M_PI / 180, flag
    );
}

int Pacman::getNumLives() const
{
    return numLives_;
}

void Pacman::wound()
{
    --numLives_;
    mIsWounded = true;
    mSourceX = 0;
    mSourceY = 1;
}

void Pacman::healWound()
{
    mIsWounded = false;
    mSourceX = 0;
    mSourceY = 0;
}

void Pacman::reset()
{
    Entity::resetPosition();
    mScore = 0;
    mNumEatenFruits = 0;
    numLives_ = INIT_LIVES_NUMBER;
}

bool Pacman::isAlive() const
{
    return numLives_ > 0;
}

bool Pacman::isWounded() const
{
    return mIsWounded;
}

bool Pacman::isMoving() const
{
    return mIsMoving;
}

bool Pacman::stopAnimation()
{
    mSourceX = 0;
    mSourceY = 0;
}

void Pacman::setMoving(bool isMoving)
{
    mIsMoving = isMoving;
}

void Pacman::score(Fruit *fruit)
{
    assert(fruit);
    fruit->eat();
    mScore += fruit->getScoreIncrement();
    ++mNumEatenFruits;
}

int Pacman::getScore() const
{
    return mScore;
}

int Pacman::getNumEatenFruits() const
{
    return mNumEatenFruits;
}

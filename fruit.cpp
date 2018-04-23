#include "fruit.hpp"
#include <iostream>

Fruit::Fruit(int x, int y, Type type, ALLEGRO_BITMAP *bitmap) :
    mX(x), mY(y), mType(type), mBitmap(bitmap)
{}

int Fruit::getX() const
{
    return mX;
}

int Fruit::getY() const
{
    return mY;
}

int Fruit::getScoreIncrement() const
{
    return static_cast<int>(mType) + 1;
}

void Fruit::render(int tileSize)
{
    if(mBitmap != nullptr)
        al_draw_bitmap(mBitmap, mX * tileSize, mY * tileSize, NULL);
}

bool Fruit::isEaten()
{
    return mIsEaten;
}

void Fruit::eat()
{
    mIsEaten = true;
}

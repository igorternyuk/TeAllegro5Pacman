#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <ctime>
#include <cstdlib>

class Fruit
{
public:
    enum class Type
    {
        MANSANA = 5,
        FRESA = 10,
        NARANJA = 15,
        HONGO = 20,
        PERA = 25
    };
    explicit Fruit(int x, int y, Type type, ALLEGRO_BITMAP *bitmap);
    int getX() const;
    int getY() const;
    int getScoreIncrement() const;
    void render(int tileSize);
    bool isEaten();
    void eat();
private:
    int mX, mY;
    Type mType;
    bool mIsEaten = false;
    ALLEGRO_BITMAP *mBitmap;
};


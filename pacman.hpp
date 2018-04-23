#pragma once
#include "entity.hpp"

class Fruit;

class Pacman : public Entity
{
public:
    explicit Pacman(int x, int y, Direction dir, charMatrix map,
                    int numLives, ALLEGRO_BITMAP *bitmap, int sourceX = 0,
                    int sourceY = 0, int numPictInAnim = 6);
    void update();
    void render(int tileSize);
    int getNumLives() const;
    void wound();
    void healWound();
    void reset();
    bool isAlive() const;
    bool isWounded() const;
    bool isMoving() const;
    bool stopAnimation();
    void setMoving(bool isMoving);
    void score(Fruit *fruit);
    int getScore() const;
    int getNumEatenFruits() const;

private:
    const int INIT_LIVES_NUMBER;
    int numLives_;
    bool mIsMoving = false;
    bool mIsWounded = false;
    int mSourceX = 0, mSourceY = 0;
    int mNumPictInAnim = 6;
    int mScore = 0;
    int mNumEatenFruits = 0;
};


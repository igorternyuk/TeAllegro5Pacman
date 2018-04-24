#pragma once
#include "entity.hpp"

class Fruit;

class Pacman : public Entity
{
public:
    explicit Pacman(int x, int y, Direction dir, charMatrix &map,
                    int numLives, ALLEGRO_BITMAP *bitmap, int sourceX = 0,
                    int sourceY = 0, int numPictInAnim = 6);
    void handleUserInput(ALLEGRO_KEYBOARD_STATE *keyState);
    void reset();
    void animate();
    void stopAnimation();
    void setMoving(bool isMoving);
    void score(Fruit *fruit);
    void wound();
    void healWound();
    void render(int tileSize);
    bool isAlive() const;
    bool isWounded() const;
    bool isMoving() const;
    int getNumLives() const;
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


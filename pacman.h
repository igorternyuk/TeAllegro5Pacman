#ifndef PACMAN_H
#define PACMAN_H

#include "character.h"

class Pacman : public Character
{
public:
    explicit Pacman(int x, int y, Direction dir, charMatrix map, int numLives, std::string pathToSprite);
    void draw(int sourceX, int sourceY, int tileSize);
    int getNumLives() const;
    void wound();
    void healWound();
    void resetLivesNumber();
    bool isAlive() const;
    bool isWounded() const;
private:
    const int INIT_LIVES_NUMBER;
    int numLives_;
    bool isWounded_ = false;
};

#endif // PACMAN_H

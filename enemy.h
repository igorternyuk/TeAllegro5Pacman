#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"

enum Fantom {RED, GREEN};

class Enemy : public Character
{
public:
    explicit Enemy(int x, int y, Direction dir, charMatrix map, Fantom type);
    void draw(int sourceX, int sourceY, int tileSize);
    Fantom getType() const;
    void disappear();
    bool isActive();
    bool isCollisionOthers(const Enemy * const other);
private:
    Fantom type_;
    bool isDisappeared_ = false;
};

#endif // ENEMY_H

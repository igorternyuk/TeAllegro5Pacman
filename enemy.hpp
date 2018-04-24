#pragma once
#include "entity.hpp"

class Enemy : public Entity
{
public:
    enum Type { RED, GREEN };
    explicit Enemy(int x, int y, Direction dir, charMatrix &map,
                   ALLEGRO_BITMAP *bitmap, Type type);
    void render(int tileSize);
    Type getType() const;
    void disappear();
    bool isActive();
    bool collides(const Enemy * const other);
private:
    Type mType;
    bool mIsDisappeared = false;
};

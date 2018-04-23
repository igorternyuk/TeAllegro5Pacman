#include "enemy.hpp"

Enemy::Enemy(int x, int y, Direction dir, charMatrix map,
             ALLEGRO_BITMAP *bitmap, Type type) :
    Entity(x, y, dir, map, bitmap), mType(type)
{}

void Enemy::render(int tileSize)
{
    al_draw_bitmap(mBitmap, getX() * tileSize, getY() * tileSize, NULL);
}

Enemy::Type Enemy::getType() const
{
    return mType;
}

void Enemy::disappear()
{
    mIsDisappeared = true;
}

bool Enemy::isActive()
{
    return !mIsDisappeared;
}

bool Enemy::collides(const Enemy * const other)
{
    if(this == other)
        return false;
    bool isCollision;
    switch (getDirection())
    {
        case Direction::LEFT:
            isCollision = (getX() - 1 == other->getX()) &&
                    (getY() == other->getY());
            break;
        case Direction::RIGHT:
            isCollision = (getX() + 1 == other->getX()) &&
                    (getY() == other->getY());
            break;
        case Direction::UP:
            isCollision = (getX() == other->getX()) &&
                    (getY() - 1 == other->getY());
            break;
        case Direction::DOWN:
            isCollision = (getX() == other->getX()) &&
                    (getY() + 1 == other->getY());
            break;
        default:
            isCollision = false;
            break;
    }
    return isCollision;
}

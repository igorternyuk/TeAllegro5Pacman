#include "enemy.h"

Enemy::Enemy(int x, int y, Direction dir, charMatrix map, Fantom type) :
    Character(x, y, dir, map,
    (type == RED ? "Resources/Images/enemigo_rojo.png" : "Resources/Images/enemigo_verde.png")),
    type_(type)
{}

void Enemy::draw(int sourceX, int sourceY, int tileSize)
{
    al_draw_bitmap(sprite_, getX() * tileSize, getY() * tileSize, NULL);
}

Fantom Enemy::getType() const
{
    return type_;
}

void Enemy::disappear()
{
    isDisappeared_ = true;
}

bool Enemy::isActive()
{
    return !isDisappeared_;
}

bool Enemy::isCollisionOthers(const Enemy * const other)
{
    if(this == other)
        return false;
    bool result;
    switch (getDirection())
    {
        case LEFT:
            result = (getX() - 1 == other->getX()) && (getY() == other->getY());
            break;
        case RIGHT:
            result = (getX() + 1 == other->getX()) && (getY() == other->getY());
            break;
        case UP:
            result = (getX() == other->getX()) && (getY() - 1 == other->getY());
            break;
        case DOWN:
            result = (getX() == other->getX()) && (getY() + 1 == other->getY());
            break;
        default:
            result = false;
            break;
    }
   /* if(result)
    {
        std::cout << "Есть коллизия с другим призраком" << std::endl;
        std::cout << "Его координаты x = " << other->getX() << " y = " << other->getY() << std::endl;
        std::cout << "Мои координаты x = " << getX() << " y = " << getY() << std::endl;
    }*/
    return result;
}

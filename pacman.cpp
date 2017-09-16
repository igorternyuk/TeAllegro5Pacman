#include "pacman.h"

Pacman::Pacman(int x, int y, Direction dir, charMatrix map, int numLives, std::string pathToSprite) :
    Character(x, y, dir, map, "Resources/Images/pacman.png"), INIT_LIVES_NUMBER(numLives), numLives_(numLives)
{}

void Pacman::draw(int sourceX, int sourceY, int tileSize)
{
    float pacImAngle = 0.0f;
    int flag = NULL;
    switch(getDirection())
    {
        case LEFT :
            pacImAngle = 0.0f;
            break;
        case RIGHT :
            pacImAngle = 180.0f;
            flag = ALLEGRO_FLIP_VERTICAL;
            break;
        case UP :
            pacImAngle = 90.0f;
            break;
        case DOWN :
            pacImAngle = 270.0f;
            break;
    }
    const float pacmanScale = 1.0f;
    ALLEGRO_COLOR filtredColor = al_map_rgb(255, 255, 255);
    al_draw_tinted_scaled_rotated_bitmap_region(sprite_, sourceX * tileSize,
    sourceY * tileSize, tileSize, tileSize, filtredColor, tileSize / 2, tileSize / 2,
    getX() * tileSize + tileSize / 2, getY() * tileSize + tileSize / 2, pacmanScale, pacmanScale,
                                                pacImAngle * M_PI / 180, flag);
}

int Pacman::getNumLives() const
{
    return numLives_;
}

void Pacman::wound()
{
    --numLives_;
    isWounded_ = true;
}

void Pacman::healWound()
{
    isWounded_ = false;
}

void Pacman::resetLivesNumber()
{
    numLives_ = INIT_LIVES_NUMBER;
}

bool Pacman::isAlive() const
{
    return numLives_ > 0;
}

bool Pacman::isWounded() const
{
    return isWounded_;
}

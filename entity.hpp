#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <vector>
#include <string>

typedef std::vector<std::string> charMatrix;
enum class Direction {LEFT, RIGHT, UP, DOWN };

class Entity
{
public:
    explicit Entity(int x, int y, Direction dir, charMatrix& map,
                    ALLEGRO_BITMAP *bitmap);
    virtual ~Entity() {}
    int getX() const;
    int getY() const;
    void setMap(const charMatrix &map);
    void setDirection(Direction dir);
    Direction getDirection() const;
    const charMatrix &getMap() const;
    void move();
    void resetPosition();
    bool isCollision(int entityX, int entityY);
    void chooseRandomDir();
    bool collidesWall();
    bool isPossibleDirection(Direction dir);
    void chooseShortestWay(int targetX, int targetY);
    virtual void render(int tileSize) = 0;

private:
    int mX, mY;
    const int mInitialX;
    const int mInitialY;
    Direction mDir;
    charMatrix &mMap;

protected:    
    ALLEGRO_BITMAP* mBitmap;

private:
    float caclDistanceToTheTarget(Direction dir, int targetX, int targetY);
};


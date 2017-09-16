#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

typedef std::vector<std::string> charMatrix;
enum Direction {LEFT, RIGHT, UP, DOWN, STOP};

class Character
{
public:
    explicit Character(int x, int y, Direction dir, charMatrix map, std::string pathToImage);
    virtual ~Character();
    int getX() const;
    int getY() const;
    void changeMap(charMatrix map);
    void setDirection(Direction dir);
    Direction getDirection() const;
    charMatrix getMap() const;
    void move();
    void resetPosition();
    bool isCollision(int entityX, int entityY);
    virtual void draw(int sourceX, int sourceY, int tileSize) = 0;
protected:
    ALLEGRO_BITMAP* sprite_;
private:
    int x_, y_;
    Direction dir_;
    charMatrix map_;
    const int INIT_PAC_X;
    const int INIT_PAC_Y;
};

#endif // CHARACTER_H

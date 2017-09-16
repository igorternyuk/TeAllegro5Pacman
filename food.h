#ifndef FOOD_H
#define FOOD_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <ctime>
#include <cstdlib>

enum FoodType {MANSANA = 5, FRESA = 10, NARANJA = 15, HONGO = 20, PERA = 25};
class Food
{
public:
    explicit Food(int x, int y);
    ~Food();
    int getX() const;
    int getY() const;
    int getScoreIncrement() const;
    void draw(int tileSize);
    bool isEaten();
    void eat();
private:
    int x_, y_;
    FoodType type_;
    bool isEaten_ = false;
    ALLEGRO_BITMAP *image_;
};

#endif // FOOD_H

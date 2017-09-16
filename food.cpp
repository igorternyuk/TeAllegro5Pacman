#include "food.h"
#include <iostream>

Food::Food(int x, int y) :
    x_(x), y_(y)
{
    //srand(time(NULL));
    int randNumber = rand() % 101;
    //std::cout << "rand = " << randNumber << std::endl;
    if(randNumber >= 0 && randNumber < 20)
    {
        type_ = MANSANA;
        image_ = al_load_bitmap("Resources/Images/mansana.png");
    }
    else if(randNumber >= 20 && randNumber < 40)
    {
        type_ = FRESA;
        image_ = al_load_bitmap("Resources/Images/fresa.png");
    }
    else if(randNumber >= 40 && randNumber < 60)
    {
        type_ = NARANJA;
        image_ = al_load_bitmap("Resources/Images/naranja.png");
    }
    else if(randNumber >= 60 && randNumber < 80)
    {
        type_ = HONGO;
        image_ = al_load_bitmap("Resources/Images/hongo.png");
    }
    else if(randNumber >= 80 && randNumber < 101)
    {
        type_ = PERA;
        image_ = al_load_bitmap("Resources/Images/pera.png");
    }
}

Food::~Food()
{
   // std::cout << "Работает деструктор еды" << std::endl;
    if(image_ != nullptr)
    {
        al_destroy_bitmap(image_);
       // std::cout << "Удалили картинку еды еды" << std::endl;
    }
}

int Food::getX() const
{
    return x_;
}

int Food::getY() const
{
    return y_;
}

int Food::getScoreIncrement() const
{
    return type_;
}

void Food::draw(int tileSize)
{
    if(image_ != nullptr)
        al_draw_bitmap(image_, x_ * tileSize, y_ * tileSize, NULL);
}

bool Food::isEaten()
{
    return isEaten_;
}

void Food::eat()
{
    isEaten_ = true;
}

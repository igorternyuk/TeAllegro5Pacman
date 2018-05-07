#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "my_untils.hpp"

class Allegro5Bitmap
{
public:
    explicit Allegro5Bitmap();
    bool loadFromFile(const std::string &fileName);
    ALLEGRO_BITMAP* get() const;
    int width() const;
    int height() const;
private:
    my_unique_ptr<ALLEGRO_BITMAP> mBitmap;
};

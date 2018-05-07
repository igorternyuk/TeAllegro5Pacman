#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include "my_untils.hpp"

class Allegro5Font
{
public:
    explicit Allegro5Font();
    bool loadFromFile(const std::string &fileName, const int &size = 24);
    ALLEGRO_FONT* get() const;
private:
    my_unique_ptr<ALLEGRO_FONT> mFont;
};


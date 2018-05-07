#include "allegro5font.hpp"
#include <memory>

Allegro5Font::Allegro5Font()
{}

bool Allegro5Font::loadFromFile(const std::string &fileName, const int &size)
{
    mFont.reset(nullptr);
    auto font = al_load_font(fileName.c_str(), size, 0);
    if(!font)
        return false;
    my_unique_ptr<ALLEGRO_FONT> ptr{ font, al_destroy_font };
    mFont.swap(ptr);
    return true;
}

ALLEGRO_FONT *Allegro5Font::get() const
{
    return mFont.get();
}

#include "allegro5bitmap.hpp"

Allegro5Bitmap::Allegro5Bitmap()
{}

bool Allegro5Bitmap::loadFromFile(const std::__cxx11::string &fileName)
{
    mBitmap.reset(nullptr);
    auto bitmap = al_load_bitmap(fileName.c_str());
    if(!bitmap)
        return false;
    my_unique_ptr<ALLEGRO_BITMAP> ptr {bitmap, al_destroy_bitmap};
    mBitmap.swap(ptr);
    return true;
}

ALLEGRO_BITMAP *Allegro5Bitmap::get() const
{
    return mBitmap.get();
}

int Allegro5Bitmap::width() const
{
    return al_get_bitmap_width(mBitmap.get());
}

int Allegro5Bitmap::height() const
{
    return al_get_bitmap_height(mBitmap.get());
}

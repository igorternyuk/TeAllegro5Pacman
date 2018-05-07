#include "allegro5sample.hpp"
#include <iostream>

Allegro5Sample::Allegro5Sample()
{}

bool Allegro5Sample::loadFromFile(const std::__cxx11::string &fileName)
{
    mSample.reset(nullptr);
    auto sound = al_load_sample(fileName.c_str());
    if(!sound)
    {
        return false;
    }
    my_unique_ptr<ALLEGRO_SAMPLE> ptr{ sound, al_destroy_sample };
    mSample.swap(ptr);
    return true;
}

ALLEGRO_SAMPLE *Allegro5Sample::get() const
{
    return mSample.get();
}

void Allegro5Sample::play()
{
    al_play_sample(mSample.get(), 1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "my_untils.hpp"

class Allegro5Sample
{
public:
    explicit Allegro5Sample();
    bool loadFromFile(const std::string &fileName);
    ALLEGRO_SAMPLE* get() const;
    void play();
private:
    my_unique_ptr<ALLEGRO_SAMPLE> mSample;
};

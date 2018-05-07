#include "allegro5initializer.hpp"
#include <stdexcept>

Allegro5Initializer::Allegro5Initializer()
{
    if(!al_init())
    {
        throw std::runtime_error("Could not initialize allegro");
    }

    if(!al_init_image_addon())
    {
        throw std::runtime_error("Could not initialize image addon");
    }

    if(!al_init_primitives_addon())
    {
        throw std::runtime_error("Could not initialize primitives addon");
    }

    al_init_font_addon();

    if(!al_init_ttf_addon())
    {
        throw std::runtime_error("Could not install ttf addon");
    }

    if(!al_install_keyboard())
    {
        throw std::runtime_error("Could not install keyboard");
    }

    if(!al_install_mouse())
    {
        throw std::runtime_error("Could not install mouse");
    }

    if(!al_install_audio())
    {
        throw std::runtime_error("Could not install audio");
    }

    if(!al_init_acodec_addon()){
        throw std::runtime_error("Could not initialize acodec addon");
    }
}

Allegro5Initializer::~Allegro5Initializer()
{
    al_uninstall_audio();
    al_uninstall_mouse();
    al_uninstall_keyboard();
}

#include "game.hpp"

int main()
{
    /*
    1. Попробовать мультимэп
    2. Попробовать умные указатели для фруктов и призраков и пакмена
    3. Сделать объект инициализирующий алегро*/


    try
    {
        Game game;
        game.run();
    }
    catch(std::exception ex)
    {
        al_show_native_message_box(NULL, NULL, NULL, ex.what(), NULL, 0);
    }

   return 0;
}


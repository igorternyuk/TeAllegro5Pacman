#include "game.hpp"
#include <iostream>

int main()
{
    try
    {
        Game game;
        game.run();
    }
    catch(std::exception ex)
    {
        al_show_native_message_box(NULL, NULL, NULL, ex.what(), NULL, 0);
        std::cout << ex.what() << std::endl;
    }

   return 0;
}


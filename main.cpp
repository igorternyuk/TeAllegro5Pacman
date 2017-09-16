#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include "pacman.h"
#include "enemy.h"
#include "food.h"

bool loadSettings(int &initPacmanX, int &initPacmanY, int &pacmanLivesCount, int &enemiesCount,
                  int &foodAmount, const std::string &fileName, charMatrix &map, ALLEGRO_BITMAP *&wall);
void createEnemies(std::vector<Enemy*> &enemies, const int &numEnemies,
                   const charMatrix &map, const int &pacX, const int &pacY);
void addFood(std::vector<Food*> &food, const int &numFood, const charMatrix &map, const int &pacX, const int &pacY);
bool isPossibleDirection(Direction dir, int curX, int curY, const charMatrix &map);
Direction chooseShortestWay(int curX, int curY, int velX, int velY, int pacX, int pacY,
                            const charMatrix &map);
Direction chooseRandomDir(int curX, int curY, const charMatrix &map);
void resolveCollision(Enemy *&enemy, Pacman &pacman, int &sourceX, int &sourceY,
                      ALLEGRO_SAMPLE *sound);
void drawMap(ALLEGRO_BITMAP *wall, const charMatrix &map);
void drawPacmanScore(int score, int left, int top, ALLEGRO_FONT *font);
void drawPacmanLives(int numLives, const int &windowWidth, const int &windowHeight, ALLEGRO_FONT *font);
void drawMessage(const int &textLeft, const int &textTop, const std::string &text,
                 ALLEGRO_COLOR color, ALLEGRO_FONT *font);
void prepareNewGame(bool &isPacmanActive, int &sourceX, int &sourceY, int &score,
                    bool &isGamePaused, bool &isWin, bool &isLost, int &numEatenFruits,
                    const charMatrix &map, Pacman &pacman, std::vector<Food*> &food,
                    const int &foodAmount, std::vector<Enemy*> &enemies, const int &enemyCount);
int main()
{
    const int SCREEN_WIDTH  = 928;
    const int SCREEN_HEIGHT  = 690;
    const int WINDOW_LEFT = 200;
    const int WINDOW_TOP = 20;
    const char* WINDOW_TITLE = "Pacman";
    const std::string GAME_PAUSED_TEXT = "GAME PAUSED";
    const std::string WIN_MESSAGE_TEXT = "CONGRATULATIONS!!!YOU WON!!!";
    const std::string LOST_MESSAGE_TEXT = "YOU LOST!!!";
    if(!al_init())
    {
        al_show_native_message_box(NULL,NULL,NULL, "Could not initialize Allegro 5", NULL, 0);
        return -1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);

    if(!display)
    {
         al_show_native_message_box(NULL, NULL, NULL, "Could not create Allegro Window", NULL,0);
    }
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    al_set_window_position(display, WINDOW_LEFT, WINDOW_TOP);
    al_set_window_title(display, WINDOW_TITLE);
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_install_keyboard();
    al_install_mouse();

    int initPacmanX, initPacmanY, pacmanLivesCount, enemiesCount, foodAmount;
    ALLEGRO_BITMAP *wall = nullptr;
    charMatrix map;
    bool done = false;
    if(!loadSettings(initPacmanX, initPacmanY, pacmanLivesCount,
                     enemiesCount, foodAmount, "settings.txt", map, wall))
    {
        al_show_native_message_box(NULL, NULL, NULL, "Could not load settings", NULL,0);
        done = true;
    }
    bool draw = false;
    bool isPacmanActive = false;
    int sourceX = 0, sourceY = 0;
    int numPictInAnim = 6;
    int score = 0;
    bool isGamePaused = false;
    bool isWin = false;
    bool isLost = false;
    int numEatenFruits = 0;

    // loads sound effect

    ALLEGRO_SAMPLE *soundPacmanChomp = al_load_sample("Resources/Sounds/pacman_chomp.wav");
    ALLEGRO_SAMPLE *soundPacmanDeath = al_load_sample("Resources/Sounds/pacman_death.wav");
    al_reserve_samples(3);
    ALLEGRO_SAMPLE *music = al_load_sample("Resources/Sounds/mario_background.ogg");      // Loads background music
    ALLEGRO_SAMPLE_INSTANCE *marioBackground = al_create_sample_instance(music);
    al_set_sample_instance_playmode(marioBackground , ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(marioBackground , al_get_default_mixer());
    //al_reserve_samples(3);

    //Here we create multiple timers

    const float pacMovingSpeed = 4.0f;    // Скорость движения пакмена
    ALLEGRO_TIMER *timer = al_create_timer(1 / pacMovingSpeed);
    const float fantomMovingSpeed = 1.0f; // Скорость фантомов
    ALLEGRO_TIMER *fantomTimer = al_create_timer(1 / fantomMovingSpeed);
    const float pacAnimSpeed = 10.0f;     // Скорость анимации пакмена
    ALLEGRO_TIMER *frameTimer = al_create_timer(1 / pacAnimSpeed);

    ALLEGRO_KEYBOARD_STATE keyState;
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_timer_event_source(fantomTimer ));
    al_register_event_source(event_queue, al_get_timer_event_source(frameTimer));
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    // Here we created font with different sizes

    ALLEGRO_FONT *font = al_load_font("Resources/Fonts/DroidSansMono.ttf", 36, 0);
    ALLEGRO_FONT *middleFont = al_load_font("Resources/Fonts/DroidSansMono.ttf", 48, 0);
    ALLEGRO_FONT *largeFont = al_load_font("Resources/Fonts/DroidSansMono.ttf", 120, 0);

    Pacman pacman(initPacmanX, initPacmanY, STOP, map, pacmanLivesCount, "Resources/Images/pacman.png");
    std::vector<Food*> food;
    std::vector<Enemy*> enemies;
    prepareNewGame(isPacmanActive, sourceX, sourceY, score, isGamePaused, isWin, isLost,
                   numEatenFruits, map, pacman, food, foodAmount, enemies, enemiesCount);

    al_start_timer(timer);
    al_start_timer(fantomTimer);
    al_start_timer(frameTimer);
    al_play_sample_instance(marioBackground);
    //main loop of the game

    while(!done) {
        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue, &events);
        al_get_keyboard_state(&keyState);
        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                done = true;
                break;
            }
            else if(events.keyboard.keycode == ALLEGRO_KEY_SPACE)
            {
                //Using space key we can pause or resume our game
                isGamePaused = !isGamePaused;
                if(isGamePaused)
                {
                    al_stop_timer(timer);
                    al_stop_timer(fantomTimer);
                    al_stop_timer(frameTimer);
                }
                else
                {
                    al_start_timer(timer);
                    al_start_timer(fantomTimer);
                    al_start_timer(frameTimer);
                }
            }
            else if(events.keyboard.keycode == ALLEGRO_KEY_N)
            {
                prepareNewGame(isPacmanActive, sourceX, sourceY, score, isGamePaused, isWin, isLost,
                               numEatenFruits, map, pacman, food, foodAmount, enemies, enemiesCount);
            }
        }
        else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
            break;
        }
        else if(events.type == ALLEGRO_EVENT_TIMER)
        {
            if(events.timer.source == timer)
            {
                isPacmanActive = true;
                if(al_key_down(&keyState, ALLEGRO_KEY_LEFT))
                {
                    pacman.setDirection(LEFT);
                    pacman.move();
                }
                else if(al_key_down(&keyState, ALLEGRO_KEY_RIGHT))
                {
                    pacman.setDirection(RIGHT);
                    pacman.move();
                }
                else if(al_key_down(&keyState, ALLEGRO_KEY_UP))
                {
                    pacman.setDirection(UP);
                    pacman.move();
                }
                else if(al_key_down(&keyState, ALLEGRO_KEY_DOWN))
                {
                    pacman.setDirection(DOWN);
                    pacman.move();
                }
                else
                {
                   isPacmanActive = false;
                }
                //Checking for pacman and food collisions
                for(auto &f : food)
                {
                    if(!f->isEaten() && pacman.isCollision(f->getX(), f->getY()))
                    {
                        f->eat();
                        score += f->getScoreIncrement();
                        ++numEatenFruits;
                        al_play_sample(soundPacmanChomp, 1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                    }
                }

                //Checking for pacman and fantoms collisions

                for(auto &fantom : enemies)
                    if(fantom->isActive() && pacman.isCollision(fantom->getX(), fantom->getY()))
                    {
                        resolveCollision(fantom, pacman, sourceX, sourceY, soundPacmanDeath);
                        al_stop_timer(fantomTimer);
                        break;
                    }

                //Checking for a win

                if(pacman.isAlive() && numEatenFruits == foodAmount)
                {
                    isWin = true;
                    al_stop_timer(fantomTimer);
                    al_stop_timer(timer);
                    al_stop_timer(frameTimer);
                }
                if(!pacman.isAlive())
                {
                    isLost = true;
                    al_stop_timer(fantomTimer);
                    al_stop_timer(timer);
                    al_stop_timer(frameTimer);
                }
            }
            else if(events.timer.source == fantomTimer)
            {
                for(auto &fantom : enemies)
                {
                    if(fantom->isActive())
                    {
                           bool isPossibleDir = isPossibleDirection(fantom->getDirection(), fantom->getX(), fantom->getY(), map);
                           bool isCrossing = map[fantom->getY()][fantom->getX()] == '+';
                           if(!isPossibleDir || isCrossing)
                           {
                                if(fantom->getType() == RED)
                                    fantom->setDirection(chooseShortestWay(fantom->getX(), fantom->getY(), 1, 1,
                                    pacman.getX(), pacman.getY(), map));
                                else if(fantom->getType() == GREEN)
                                    fantom->setDirection(chooseRandomDir(fantom->getX(), fantom->getY(), map));
                           }

                        //Checking for collisions of current fantom with other ones

                        for(auto &otherFantom : enemies)
                        {
                            if(otherFantom->isActive() &&
                               fantom->isCollisionOthers(otherFantom))
                            {
                                switch (fantom->getDirection()) {
                                case LEFT:
                                    fantom->setDirection(RIGHT);
                                    break;
                                case RIGHT:
                                    fantom->setDirection(LEFT);
                                    break;
                                case UP:
                                    fantom->setDirection(DOWN);
                                    break;
                                case DOWN:
                                    fantom->setDirection(UP);
                                    break;
                                default:
                                    break;
                                }
                                break;
                            }
                        }
                        fantom->move();
                        // Checking for collisions fantoms with pacman
                        if(fantom->isCollision(pacman.getX(), pacman.getY()))
                        {
                            resolveCollision(fantom, pacman, sourceX, sourceY, soundPacmanDeath);
                            al_stop_timer(fantomTimer);
                        }
                    }
                }
            }
            else if(events.timer.source == frameTimer)
            {
                //Обработка события таймера анимации пакмена
                if(isPacmanActive || pacman.isWounded())
                {
                    ++sourceX;
                    if(sourceX >= numPictInAnim)
                    {
                        sourceX = 0;
                        sourceY = 0;
                        if(pacman.isWounded())
                        {
                            pacman.resetPosition();
                            pacman.healWound();
                            al_start_timer(fantomTimer);
                        }
                    }
                }
                else
                {
                   sourceX = 0;
                }
            }
            draw = true;
        }

        if(draw) {
            al_clear_to_color(al_map_rgb(0,0,0));
            drawMap(wall, map);
            //Прорисовываем еду если она не сьедена
            for(auto &f : food)
                if(!f->isEaten())
                    f->draw(32);
            //Прорисовываем пакмена
            pacman.draw(sourceX, sourceY, 32);
            //Прорисовываем врагов
            for(auto &fantom : enemies)
            {
                if(fantom->isActive())
                    fantom->draw(0, 0, 32);
            }
            drawPacmanScore(score, 20 , SCREEN_HEIGHT - 45, font);
            drawPacmanLives(pacman.getNumLives(), SCREEN_WIDTH, SCREEN_HEIGHT, font);
            if(isGamePaused && !isWin && !isLost)
            {
                drawMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120, GAME_PAUSED_TEXT, al_map_rgb(255,255,0), largeFont);
            }
            else if(isWin)
            {
                drawMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120, WIN_MESSAGE_TEXT, al_map_rgb(255,255,0), middleFont);
            }
            else if(isLost)
            {
                drawMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120, LOST_MESSAGE_TEXT, al_map_rgb(255,255,255), largeFont);
            }
            al_flip_display();
        }
    }
    for(auto f : food)
        if(f != nullptr)
            delete f;
    for(auto e : enemies)
        if(e != nullptr)
            delete e;

    al_destroy_display(display);
    al_destroy_sample(soundPacmanChomp);
    al_destroy_sample(soundPacmanDeath);
    al_destroy_sample(music);
    al_destroy_sample_instance(marioBackground);
    al_destroy_bitmap(wall);
    al_destroy_font(font);
    al_destroy_font(middleFont);
    al_destroy_font(largeFont);
    al_destroy_timer(timer);
    al_destroy_timer(fantomTimer);
    al_destroy_timer(frameTimer);
    al_destroy_event_queue(event_queue);

    return 0;
}
bool loadSettings(int &initPacmanX, int &initPacmanY,
                  int &pacmanLivesCount, int &enemiesCount, int &foodAmount,
                  const std::string &fileName, charMatrix &map, ALLEGRO_BITMAP *&wall)
{
    enum LoadState {INIT_PACMAN_X, INIT_PACMAN_Y, NUM_PACMAN_LIVES, ENEMIES_NUMBER,
                    FOOD_AMOUNT, TILE_SET, MAP};
    int state = 0;
    std::ifstream fi(fileName);
    if(!map.empty())
        map.clear();
    if(fi.is_open())
    {
       while(!fi.eof())
       {
           std::string line;
           std::getline(fi, line);
           if(line.length() == 0)
               continue;
           if(line == "[InitialPacmanX]")
           {
               state = INIT_PACMAN_X;
               continue;
           }
           else if(line == "[InitialPacmanY]")
           {
               state = INIT_PACMAN_Y;
               continue;
           }
           else if(line == "[NumberOfPacmanLives]")
           {
               state = NUM_PACMAN_LIVES;
               continue;
           }
           else if(line == "[EnemiesNumber]")
           {
               state = ENEMIES_NUMBER;
               continue;
           }
           else if(line == "[FoodAmount]")
           {
               state = FOOD_AMOUNT;
               continue;
           }
           else if(line == "[TileSet]")
           {
               state = TILE_SET;
               continue;
           }
           else if(line == "[Map]")
           {
               state = MAP;
               continue;
           }
           std::stringstream stream;
           switch(state)
           {
               case INIT_PACMAN_X :
                   //std::stringstream stream;
                   stream << line;
                   stream >> initPacmanX;
                   break;
               case INIT_PACMAN_Y :
                   //std::stringstream stream;
                   stream << line;
                   stream >> initPacmanY;
                   break;
               case NUM_PACMAN_LIVES :
                   //std::stringstream stream;
                   stream << line;
                   stream >> pacmanLivesCount;
                   break;
               case ENEMIES_NUMBER :
                   //std::stringstream stream;
                   stream << line;
                   stream >> enemiesCount;
                   break;
               case FOOD_AMOUNT :
                   //std::stringstream stream;
                   stream << line;
                   stream >> foodAmount;
                   break;
               case TILE_SET :
                   wall = al_load_bitmap(line.c_str());
                   break;
               case MAP :
                   map.push_back(line);
                   break;
               default:
                   break;
           }
       }
       fi.close();
       return !map.empty() && wall != nullptr;
    }
    else
    {
        al_show_native_message_box(NULL, NULL, NULL, "Could not open the map file!", NULL, 0);
        return false;
    }

}

void drawMap(ALLEGRO_BITMAP *wall, const charMatrix &map)
{
    for(int row = 0; row < int(map.size()); ++row)
    {
        for(int col = 0; col < int(map[row].size()); ++col)
        {
            if(map[row][col] == 'X'){
               al_draw_bitmap(wall, col * al_get_bitmap_width(wall), row * al_get_bitmap_height(wall), 0);

            }
        }
    }
}

void createEnemies(std::vector<Enemy*> &enemies, const int &numEnemies,
                   const charMatrix &map, const int &pacX, const int &pacY)
{
    for(int i = 0; i < numEnemies; ++i)
    {
        int randX, randY;
        bool isGoodPosition;
        do
        {
            isGoodPosition = true;
            randY = rand() % map.size();
            randX = rand() % map[randY].size();
            bool isWall = map[randY][randX] == 'X';
            bool isPacPos = (randX == pacX) && (randY == pacY);
            if(isWall || isPacPos)
                isGoodPosition = false;
            for(const auto &e : enemies)
                if(randX == e->getX() && randY == e->getY())
                {
                    isGoodPosition = false;
                    break;
                };
        } while(!isGoodPosition);
        Fantom type = rand() % 100 < 50 ? RED : GREEN;
        Direction randDir = chooseRandomDir(randX, randY, map);
        Enemy *e = new Enemy(randX, randY, randDir, map, type);
        enemies.push_back(e);
    }
}

void addFood(std::vector<Food*> &food, const int &numFood, const charMatrix &map, const int &pacX, const int &pacY)
{
    for(int i = 0; i < numFood; ++i)
    {
        int randX, randY;
        bool isGoodPosition;
        do
        {
            isGoodPosition = true;
            randY = rand() % map.size();
            randX = rand() % map[randY].size();
            bool isWall = map[randY][randX] == 'X';
            bool isPacPos = (randX == pacX) && (randY == pacY);
            if (isWall || isPacPos)
                isGoodPosition = false;
            for(const auto &f : food)
                if(randX == f->getX() && randY == f->getY())
                {
                    isGoodPosition = false;
                    break;
                }
        } while(!isGoodPosition);
        Food *f = new Food(randX, randY);
        food.push_back(f);
    }
}

bool isPossibleDirection(Direction dir, int curX, int curY, const charMatrix &map)
{
    switch(dir)
    {
         case LEFT :
             --curX;
             break;
         case RIGHT :
             ++curX;
             break;
         case UP :
             --curY;
             break;
         case DOWN :
             ++curY;
             break;
         default:
             break;
    }
    return curY >= 0 && curY < int(map.size()) && curX >= 0 && curX < int(map[curY].size()) && (map[curY][curX] != 'X');
}

Direction chooseShortestWay(int curX, int curY, int velX, int velY, int pacX, int pacY, const charMatrix &map)
{
   //std::cout << "--------------------" << std::endl;
   //std::cout << "x = " << curX << " y = " << curY << std::endl;
   std::vector<std::pair<Direction, float>> distances;
   float leftDistance = (curX - velX - pacX) * (curX - velX - pacX) + (curY - pacY) * (curY - pacY);
   //std::cout << "left = " << leftDistance << std::endl;
   std::pair<Direction, float> left(LEFT, leftDistance);
   distances.push_back(left);
   float rightDistance = (curX + velX - pacX) * (curX + velX - pacX) + (curY - pacY) * (curY - pacY);
  // std::cout << "right = " << rightDistance << std::endl;
   std::pair<Direction, float> right(RIGHT, rightDistance);
   distances.push_back(right);
   float upDistance = (curX - pacX) * (curX - pacX) + (curY - velY - pacY) * (curY - velY - pacY);
   //std::cout << "up = " << rightDistance << std::endl;
   std::pair<Direction, float> up(UP, upDistance);
   distances.push_back(up);
   float downDistance = (curX - pacX) * (curX - pacX) + (curY + velY - pacY) * (curY + velY - pacY);
   //std::cout << "down = " << downDistance << std::endl;
   std::pair<Direction, float> down(DOWN, downDistance);
   distances.push_back(down);
   std::sort(distances.begin(), distances.end(), [](auto a, auto b){ return a.second < b.second;});
   int index = 0;
   isPossibleDirection(distances[index].first, curX, curY, map);
   while(!isPossibleDirection(distances[index].first, curX, curY, map)) {++index;};
   return distances[index].first;
}

Direction chooseRandomDir(int curX, int curY, const charMatrix &map)
{
   Direction randDir;
   bool isDirOK = true;
   do
   {
       int randNumber = rand() % 1000;
       if(randNumber >= 0 && randNumber < 250)
           randDir = LEFT;
       else if(randNumber >= 250 && randNumber < 500)
           randDir = RIGHT;
       else if(randNumber >= 500 && randNumber < 750)
           randDir = UP;
       else if(randNumber >= 750 && randNumber < 1000)
           randDir = DOWN;
       isDirOK = isPossibleDirection(randDir, curX, curY, map);
   } while(!isDirOK);
   return randDir;
}

void resolveCollision(Enemy *&enemy, Pacman &pacman, int &sourceX, int &sourceY, ALLEGRO_SAMPLE *sound)
{
    pacman.setDirection(LEFT);
    pacman.wound();
    sourceX = 0;
    sourceY = 1;
    enemy->disappear();
    al_play_sample(sound, 1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

void drawPacmanScore(int score, int left, int top, ALLEGRO_FONT *font)
{
    std::stringstream ss;
    ss << "Score: " << score;
    al_draw_text(font, al_map_rgb(255,50,0), left , top, ALLEGRO_ALIGN_LEFT, ss.str().c_str());
}

void drawPacmanLives(int numLives, const int &windowWidth, const int &windowHeight, ALLEGRO_FONT *font)
{
    al_draw_text(font, al_map_rgb(70, 80, 255), windowWidth - 160 / 5 * numLives, windowHeight - 50, ALLEGRO_ALIGN_RIGHT, "Pacman lives: ");
    const int circleRadius = 15;
    for(int i = 1; i <= numLives; ++i)
    {
        ALLEGRO_COLOR circleColor = al_map_rgb(255,255,0);
        al_draw_filled_circle(windowWidth - i * (2 * circleRadius + 3), windowHeight - 25, circleRadius, circleColor);
    }
}

void drawMessage(const int &textLeft, const int &textTop, const std::string &text, ALLEGRO_COLOR color, ALLEGRO_FONT *font)
{
    al_draw_text(font, color, textLeft, textTop, ALLEGRO_ALIGN_CENTER, text.c_str());
}

void prepareNewGame(bool &isPacmanActive, int &sourceX, int &sourceY, int &score, bool &isGamePaused,
                  bool &isWin, bool &isLost, int &numEatenFruits, const charMatrix &map, Pacman &pacman,
                  std::vector<Food*> &food, const int &foodAmount, std::vector<Enemy*> &enemies, const int &enemyCount)
{
    isPacmanActive = false;
    sourceX = 0;
    sourceY = 0;
    score = 0;
    isGamePaused = false;
    isWin = false;
    isLost = false;
    numEatenFruits = 0;
    pacman.resetPosition();
    pacman.resetLivesNumber();
    for(auto f : food)
        if(f != nullptr)
            delete f;
    if(!food.empty())
        food.clear();
    for(auto e : enemies)
        if(e != nullptr)
            delete e;
    if(!enemies.empty())
        enemies.clear();
    addFood(food, foodAmount, map, pacman.getX(), pacman.getY());
    createEnemies(enemies, enemyCount, map, pacman.getX(), pacman.getY());
}

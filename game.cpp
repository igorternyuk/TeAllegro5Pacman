#include "game.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

Game::Game()
{
    //Allegro5 initialization
    if(!al_init())
    {
        throw std::runtime_error("Could not initialize Allegro5");
    }
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_install_keyboard();
    al_install_mouse();

    mDisplay = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);

    if(!mDisplay)
    {
         throw std::runtime_error("Could not create Allegro Window");
    }

    al_set_new_display_flags(ALLEGRO_RESIZABLE);

    al_set_window_position(mDisplay, WINDOW_LEFT, WINDOW_TOP);
    al_set_window_title(mDisplay, WINDOW_TITLE);

    loadSettings(PATH_TO_SETTINGS_FILE);

    loadBitmaps();

    // Here we created some fonts of different sizes
    loadFonts();

    // loads sound effects
    loadSounds();

    //Here we create multiple timers
    mTimer = al_create_timer(1 / mPacMovingSpeed);
    mFantomTimer = al_create_timer(1 / mFantomMovingSpeed);
    mFrameTimer = al_create_timer(1 / mPacAnimSpeed);

    mEventQueue = al_create_event_queue();

    al_register_event_source(mEventQueue, al_get_timer_event_source(mTimer));
    al_register_event_source(mEventQueue, al_get_timer_event_source(mFantomTimer ));
    al_register_event_source(mEventQueue, al_get_timer_event_source(mFrameTimer));
    al_register_event_source(mEventQueue, al_get_display_event_source(mDisplay));
    al_register_event_source(mEventQueue, al_get_keyboard_event_source());

    mPacman = std::make_unique<Pacman>(mInitPacmanX, mInitPacmanY, Direction::LEFT, mMap,
                               mPacmanLivesCount, mPacmanBitmap);
    prepareNewGame();
}

Game::~Game()
{
    al_destroy_timer(mTimer);
    al_destroy_timer(mFantomTimer);
    al_destroy_timer(mFrameTimer);
    al_destroy_event_queue(mEventQueue);
    destroySounds();
    destroyFonts();
    destroyBitmaps();
    al_destroy_display(mDisplay);
}

void Game::run()
{
    al_start_timer(mTimer);
    al_start_timer(mFantomTimer);
    al_start_timer(mFrameTimer);
    al_play_sample_instance(mBackgroundMusicInstance);

    //main loop of the game
    while(mIsRunning) {
       update();
       render();
    }
}

void Game::update()
{
    ALLEGRO_EVENT events;
    al_wait_for_event(mEventQueue, &events);
    al_get_keyboard_state(&mKeyState);
    if(events.type == ALLEGRO_EVENT_KEY_UP)
    {
        if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        {
            mIsRunning = false;
        }
        else if(events.keyboard.keycode == ALLEGRO_KEY_SPACE)
        {
            togglePause();
        }
        else if(events.keyboard.keycode == ALLEGRO_KEY_N)
        {
            prepareNewGame();
        }
    }
    else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        mIsRunning = false;
    }
    else if(events.type == ALLEGRO_EVENT_TIMER)
    {
        if(events.timer.source == mTimer)
        {
            mPacman->handleUserInput(&mKeyState);
            checkPacmanCollisions();
            checkGameState();
        }
        else if(events.timer.source == mFantomTimer)
        {
            checkFantomCollisions();
        }
        else if(events.timer.source == mFrameTimer)
        {
            if(mPacman->isMoving() || mPacman->isWounded())
            {
                mPacman->animate();
            }
            else
            {
                mPacman->stopAnimation();
            }
        }
        mRender = true;
    }
}

void Game::render()
{
    if(mRender) {
        al_clear_to_color(al_map_rgb(0,0,0));

        renderMap(mWallBitmap, mMap);

        for(auto &f : mFruits)
            if(!f->isEaten())
                f->render(TILE_SIZE);

        mPacman->render(TILE_SIZE);

        for(auto &fantom : mEnemies)
        {
            if(fantom->isActive())
                fantom->render(TILE_SIZE);
        }
        renderPacmanScore(20 , SCREEN_HEIGHT - 45, mSmallFont);
        renderPacmanLives( mSmallFont);
        if(mGameState == GameState::PAUSE)
        {
            renderTextMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120,
                        GAME_PAUSED_TEXT, al_map_rgb(255,255,0), mLargeFont);
        }
        else if(mGameState == GameState::VICTORY)
        {
            renderTextMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120,
                        WIN_MESSAGE_TEXT, al_map_rgb(255,255,0), mMiddleFont);
        }
        else if(mGameState == GameState::DEFEAT)
        {
            renderTextMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120,
                        LOST_MESSAGE_TEXT, al_map_rgb(255,255,255), mLargeFont);
        }
        al_flip_display();
    }
}

void Game::loadSettings(const std::string &fileName)
{
    enum LoadState {INIT_PACMAN_X, INIT_PACMAN_Y, NUM_PACMAN_LIVES, ENEMIES_NUMBER,
                    FOOD_AMOUNT, TILE_SET, MAP};
    int state = 0;
    std::ifstream fi(fileName);
    if(!mMap.empty())
        mMap.clear();
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
                   stream << line;
                   stream >> mInitPacmanX;
                   break;
               case INIT_PACMAN_Y :
                   stream << line;
                   stream >> mInitPacmanY;
                   break;
               case NUM_PACMAN_LIVES :
                   stream << line;
                   stream >> mPacmanLivesCount;
                   break;
               case ENEMIES_NUMBER :
                   stream << line;
                   stream >> mEnemiesCount;
                   break;
               case FOOD_AMOUNT :
                   stream << line;
                   stream >> mFoodAmount;
                   break;
               case TILE_SET :
                   mWallBitmap = al_load_bitmap(line.c_str());
                   break;
               case MAP :
                   mMap.push_back(line);
                   break;
               default:
                   break;
           }
       }
       fi.close();
       if(mMap.empty() || !mWallBitmap)
       {
           throw std::runtime_error("Incorrect map file!");
       }
    }
    else
    {
        throw std::runtime_error("Could not open the map file!");
    }

}

void Game::loadFonts()
{
    mSmallFont = al_load_font("Resources/Fonts/DroidSansMono.ttf", 28, 0);
    mMiddleFont = al_load_font("Resources/Fonts/DroidSansMono.ttf", 48, 0);
    mLargeFont = al_load_font("Resources/Fonts/DroidSansMono.ttf", 120, 0);
}

void Game::loadSounds()
{
    mSoundPacmanChomp = al_load_sample("Resources/Sounds/pacman_chomp.wav");
    mSoundPacmanDeath = al_load_sample("Resources/Sounds/pacman_death.wav");
    al_reserve_samples(3);
    // Loads background music
    mBackgroundMusicSample = al_load_sample("Resources/Sounds/mario_background.ogg");
    mBackgroundMusicInstance = al_create_sample_instance(mBackgroundMusicSample);
    al_set_sample_instance_playmode(mBackgroundMusicInstance , ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(mBackgroundMusicInstance , al_get_default_mixer());
}

void Game::loadBitmaps()
{
    mPacmanBitmap = al_load_bitmap("Resources/Images/pacman.png");
    mEnemyBitmaps.insert(std::make_pair(Enemy::Type::RED,
                                        al_load_bitmap("Resources/Images/enemigo_rojo.png")));
    mEnemyBitmaps.insert(std::make_pair(Enemy::Type::GREEN,
                                        al_load_bitmap("Resources/Images/enemigo_verde.png")));
    mFruitBitmaps.insert(std::make_pair(Fruit::Type::MANSANA,
                                        al_load_bitmap("Resources/Images/mansana.png")));
    mFruitBitmaps.insert(std::make_pair(Fruit::Type::FRESA,
                                        al_load_bitmap("Resources/Images/fresa.png")));
    mFruitBitmaps.insert(std::make_pair(Fruit::Type::NARANJA,
                                        al_load_bitmap("Resources/Images/naranja.png")));
    mFruitBitmaps.insert(std::make_pair(Fruit::Type::HONGO,
                                        al_load_bitmap("Resources/Images/hongo.png")));
    mFruitBitmaps.insert(std::make_pair(Fruit::Type::PERA,
                                        al_load_bitmap("Resources/Images/pera.png")));

}

void Game::destroyBitmaps()
{
    al_destroy_bitmap(mPacmanBitmap);
    for(auto it = mEnemyBitmaps.begin(); it != mEnemyBitmaps.end(); ++it)
    {
        al_destroy_bitmap(it->second);
    }
    for(auto it = mFruitBitmaps.begin(); it != mFruitBitmaps.end(); ++it)
    {
        al_destroy_bitmap(it->second);
    }
}

void Game::destroyFonts()
{
    al_destroy_font(mSmallFont);
    al_destroy_font(mMiddleFont);
    al_destroy_font(mLargeFont);
}

void Game::destroySounds()
{
    al_destroy_sample(mSoundPacmanChomp);
    al_destroy_sample(mSoundPacmanDeath);
    al_destroy_sample(mBackgroundMusicSample);
    al_destroy_sample_instance(mBackgroundMusicInstance);
}

void Game::renderMap(ALLEGRO_BITMAP *wall, const charMatrix &map)
{
    for(int row = 0; row < int(map.size()); ++row)
    {
        for(int col = 0; col < int(map[row].size()); ++col)
        {
            if(map[row][col] == 'X'){
               al_draw_bitmap(wall, col * al_get_bitmap_width(wall),
                              row * al_get_bitmap_height(wall), 0);

            }
        }
    }
}

void Game::createEnemies()
{
    for(int i = 0; i < mEnemiesCount; ++i)
    {
        int randX, randY;
        bool isGoodPosition;
        do
        {
            isGoodPosition = true;
            randY = rand() % mMap.size();
            randX = rand() % mMap[randY].size();
            bool isWall = mMap[randY][randX] == 'X';
            bool isPacPos = (randX == mPacman->getX()) &&
                    (randY == mPacman->getY());
            if(isWall || isPacPos)
                isGoodPosition = false;
            for(const auto &e : mEnemies)
                if(randX == e->getX() && randY == e->getY())
                {
                    isGoodPosition = false;
                    break;
                };
        } while(!isGoodPosition);
        Enemy::Type type = rand() % 100 < 50 ? Enemy::Type::RED :
                                               Enemy::Type::GREEN;
        auto enemy = std::make_unique<Enemy>(randX, randY, Direction::RIGHT, mMap,
                                      mEnemyBitmaps[type], type);
        enemy->chooseRandomDir();
        mEnemies.push_back(std::move(enemy));
    }
}

void Game::createFruits()
{
    for(int i = 0; i < mFoodAmount; ++i)
    {
        int randX, randY;
        bool isGoodPosition;
        do
        {
            isGoodPosition = true;
            randY = rand() % mMap.size();
            randX = rand() % mMap[randY].size();
            bool isWall = mMap[randY][randX] == 'X';
            bool isPacPos = (randX == mPacman->getX()) && (randY == mPacman->getY());
            if (isWall || isPacPos)
                isGoodPosition = false;
            for(const auto &f : mFruits)
                if(randX == f->getX() && randY == f->getY())
                {
                    isGoodPosition = false;
                    break;
                }
        } while(!isGoodPosition);
        int rnd = rand() % 5;
        Fruit::Type randType;
        switch(rnd)
        {
            case 0:
                randType = Fruit::Type::FRESA;
                break;
            case 1:
                randType = Fruit::Type::HONGO;
                break;
            case 2:
                randType = Fruit::Type::MANSANA;
                break;
            case 3:
                randType = Fruit::Type::NARANJA;
                break;
            default:
                randType = Fruit::Type::PERA;
                break;

        }
        auto fruit = std::make_unique<Fruit>(randX, randY, randType, mFruitBitmaps[randType]);
        mFruits.push_back(std::move(fruit));
    }
}

void Game::togglePause()
{
    //Using space key we can pause or resume our game
    //mIsGamePaused = !mIsGamePaused;
    if(mGameState == GameState::PLAYING)
    {
        mGameState = GameState::PAUSE;
        stopAllTimers();
    }
    else if(mGameState == GameState::PAUSE)
    {
        mGameState = GameState::PLAYING;
        startAllTimers();
    }
}

void Game::checkGameState()
{
    if(mPacman->isAlive() && mPacman->getNumEatenFruits() == mFoodAmount)
    {
        mGameState = GameState::VICTORY;
        stopAllTimers();
    }
    if(!mPacman->isAlive())
    {
        mGameState = GameState::DEFEAT;
        stopAllTimers();
    }
}

void Game::startAllTimers()
{
    al_start_timer(mTimer);
    al_start_timer(mFantomTimer);
    al_start_timer(mFrameTimer);
}

void Game::stopAllTimers()
{
    al_stop_timer(mFantomTimer);
    al_stop_timer(mTimer);
    al_stop_timer(mFrameTimer);
}

void Game::checkPacmanCollisions()
{
    //Checking for pacman and food collisions
    for(auto &f : mFruits)
    {
        if(!f->isEaten() && mPacman->isCollision(f->getX(), f->getY()))
        {
            mPacman->score(f.get());
            al_play_sample(mSoundPacmanChomp, 1.0f, 0.0f, 1.0,
                           ALLEGRO_PLAYMODE_ONCE, 0);
            break;
        }
    }

    //Checking for pacman and fantoms collisions

    for(const auto &fantom : mEnemies)
    {
        if(fantom->isActive() && mPacman->isCollision(fantom->getX(), fantom->getY()))
        {
            handlePacmanEnemyCollision(*fantom);
            //al_stop_timer(mFantomTimer);
            break;
        }
    }
}

void Game::checkFantomCollisions()
{
    for(auto &fantom : mEnemies)
    {
        if(fantom->isActive())
        {
           bool isCrossing = mMap[fantom->getY()][fantom->getX()] == '+';
           if(fantom->collidesWall() || isCrossing)
           {
                if(fantom->getType() == Enemy::Type::RED)
                    fantom->chooseShortestWay(mPacman->getX(), mPacman->getY());
                else if(fantom->getType() == Enemy::Type::GREEN)
                    fantom->chooseRandomDir();
           }

            //Checking for collisions of current fantom with other ones

            for(auto &otherFantom : mEnemies)
            {
                if(otherFantom->isActive() &&
                   fantom->collides(otherFantom.get()))
                {
                    switch (fantom->getDirection()) {
                    case Direction::LEFT:
                        fantom->setDirection(Direction::RIGHT);
                        break;
                    case Direction::RIGHT:
                        fantom->setDirection(Direction::LEFT);
                        break;
                    case Direction::UP:
                        fantom->setDirection(Direction::DOWN);
                        break;
                    case Direction::DOWN:
                        fantom->setDirection(Direction::UP);
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }
            fantom->move();

            // Checking for collisions fantoms with pacman
            if(fantom->isCollision(mPacman->getX(), mPacman->getY()))
            {
                handlePacmanEnemyCollision(*fantom);
            }
        }
    }
}

void Game::handlePacmanEnemyCollision(Enemy &enemy)
{
    mPacman->setDirection(Direction::LEFT);
    mPacman->wound();
    enemy.disappear();
    al_play_sample(mSoundPacmanDeath, 1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

void Game::renderPacmanScore(int left, int top, ALLEGRO_FONT *font)
{
    std::stringstream ss;
    ss << "Score: " << mPacman->getScore() << " Fruits: "<<
          mPacman->getNumEatenFruits();
    al_draw_text(font, al_map_rgb(255,50,0), left , top,
                 ALLEGRO_ALIGN_LEFT, ss.str().c_str());
}

void Game::renderPacmanLives(ALLEGRO_FONT *font)
{
    int numLives = mPacman->getNumLives();
    al_draw_text(font, al_map_rgb(70, 80, 255), SCREEN_WIDTH - 160 / 5 * numLives,
                 SCREEN_HEIGHT - 45, ALLEGRO_ALIGN_RIGHT, "Pacman lives: ");
    const int circleRadius = 15;
    for(int i = 1; i <= numLives; ++i)
    {
        ALLEGRO_COLOR circleColor = al_map_rgb(255,255,0);
        al_draw_filled_circle(SCREEN_WIDTH - i * (2 * circleRadius + 3),
                              SCREEN_HEIGHT - 25, circleRadius, circleColor);
    }
}

void Game::renderTextMessage(const int &textLeft, const int &textTop,
                       const std::string &text, ALLEGRO_COLOR color,
                       ALLEGRO_FONT *font)
{
    al_draw_text(font, color, textLeft, textTop, ALLEGRO_ALIGN_CENTER,
                 text.c_str());
}

void Game::prepareNewGame()
{
    mGameState = GameState::PLAYING;
    mPacman->reset();
    mFruits.clear();
    mEnemies.clear();
    createFruits();
    createEnemies();
    startAllTimers();
}

//For debugging only
void Game::printMap()
{
    for(auto &line: mMap)
    {
        std::cout << line << std::endl;
    }

}

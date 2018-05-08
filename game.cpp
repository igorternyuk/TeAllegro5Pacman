#include "game.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

/*
    const float mPacMovingSpeed = 4.0f;
    const float mFantomMovingSpeed = 1.0f;
    const float mPacAnimSpeed = 10.0f;
*/
Game::Game():
    mDisplay{ al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT), al_destroy_display },
    mEventQueue{ al_create_event_queue(), al_destroy_event_queue },
    mTimer{ mPacMovingSpeed },
    mFantomTimer{ mFantomMovingSpeed },
    mFrameTimer{ mPacAnimSpeed }
{
    if(!mDisplay.get())
    {
         throw std::runtime_error("Could not create Allegro Window");
    }

    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    al_set_window_position(mDisplay.get(), WINDOW_LEFT, WINDOW_TOP);
    al_set_window_title(mDisplay.get(), WINDOW_TITLE);

    loadSettings(PATH_TO_SETTINGS_FILE);

    //Load images
    loadBitmaps();

    // Here we create some fonts of different sizes
    loadFonts();

    // loads sound effects
    loadSounds();

    al_register_event_source(mEventQueue.get(),
                             mTimer.getEventSource());
    al_register_event_source(mEventQueue.get(),
                             mFantomTimer.getEventSource());
    al_register_event_source(mEventQueue.get(),
                             mFrameTimer.getEventSource());
    al_register_event_source(mEventQueue.get(),
                             al_get_display_event_source(mDisplay.get()));
    al_register_event_source(mEventQueue.get(),
                             al_get_keyboard_event_source());

    mPacman = std::make_unique<Pacman>(mInitPacmanX, mInitPacmanY,
                                       Direction::LEFT, mMap,
                                       mPacmanLivesCount,
                                       mBitmaps.get(BitmapID::Pacman).get());
    startNewGame();
}

void Game::run()
{
    al_play_sample_instance(mBackgroundMusicInstance.get());

    //main loop of the game
    while(mIsRunning) {
       update();
       render();
    }
}

void Game::update()
{
    ALLEGRO_EVENT events;
    al_wait_for_event(mEventQueue.get(), &events);
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
            startNewGame();
        }
         mRender = true;
    }
    else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        mIsRunning = false;
    }
    else if(events.type == ALLEGRO_EVENT_TIMER)
    {
        if(events.timer.source == mTimer.get())
        {
            mPacman->handleUserInput(&mKeyState);
            checkPacmanCollisions();
            checkGameState();
        }
        else if(events.timer.source == mFantomTimer.get())
        {
            checkFantomCollisions();
        }
        else if(events.timer.source == mFrameTimer.get())
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

        renderMap(mMap);

        for(auto &f : mFruits)
            if(!f->isEaten())
                f->render(TILE_SIZE);

        mPacman->render(TILE_SIZE);

        for(auto &fantom : mEnemies)
        {
            if(fantom->isActive())
                fantom->render(TILE_SIZE);
        }
        renderPacmanScore(20 , SCREEN_HEIGHT - 45,
                          mFonts.get(FontID::Small).get());
        renderPacmanLives(mFonts.get(FontID::Small).get());

        if(mGameState == GameState::PAUSE)
        {
            renderTextMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120,
                              GAME_PAUSED_TEXT, al_map_rgb(255,255,0),
                              mFonts.get(FontID::Large).get());
        }
        else if(mGameState == GameState::VICTORY)
        {
            renderTextMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120,
                              WIN_MESSAGE_TEXT, al_map_rgb(255,255,0),
                              mFonts.get(FontID::Middle).get());
        }
        else if(mGameState == GameState::DEFEAT)
        {
            renderTextMessage(SCREEN_WIDTH /  2, SCREEN_HEIGHT / 2 - 120,
                              LOST_MESSAGE_TEXT, al_map_rgb(255,255,255),
                              mFonts.get(FontID::Large).get());
        }
        al_flip_display();
        mRender = false;
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
                   mPathToWallBitmap = line;
                   break;
               case MAP :
                   mMap.push_back(line);
                   break;
               default:
                   break;
           }
       }
       fi.close();
       if(mMap.empty() || mPathToWallBitmap.empty())
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
    mFonts.load(FontID::Small, "Resources/Fonts/DroidSansMono.ttf", 28);
    mFonts.load(FontID::Middle, "Resources/Fonts/DroidSansMono.ttf", 48);
    mFonts.load(FontID::Large, "Resources/Fonts/DroidSansMono.ttf", 120);
}

void Game::loadSounds()
{
    mSounds.load(SampleID::PacmanChomp, "Resources/Sounds/pacman_chomp.wav");
    mSounds.load(SampleID::PagmanDeath, "Resources/Sounds/pacman_death.wav");
    mSounds.load(SampleID::Bgm, "Resources/Sounds/mario_background.ogg");
    al_reserve_samples(3);

    // Loads background music
    my_unique_ptr<ALLEGRO_SAMPLE_INSTANCE> bgm
    {
        al_create_sample_instance(mSounds.get(SampleID::Bgm).get()),
        al_destroy_sample_instance
    };
    al_set_sample_instance_playmode(bgm.get(), ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(bgm.get(), al_get_default_mixer());

    mBackgroundMusicInstance.swap(bgm);
}

void Game::loadBitmaps()
{
    //Wall
    mBitmaps.load(BitmapID::Wall, mPathToWallBitmap);

    //Pacman
    mBitmaps.load(BitmapID::Pacman, "Resources/Images/pacman.png");

    //Enemies
    mBitmaps.load(BitmapID::RedGhost, "Resources/Images/enemigo_rojo.png");
    mBitmaps.load(BitmapID::GreenGhost, "Resources/Images/enemigo_verde.png");

    //Fruits
    mBitmaps.load(BitmapID::Apple, "Resources/Images/mansana.png");
    mBitmaps.load(BitmapID::Pear, "Resources/Images/pera.png");
    mBitmaps.load(BitmapID::Orange, "Resources/Images/naranja.png");
    mBitmaps.load(BitmapID::Mushroom, "Resources/Images/hongo.png");
    mBitmaps.load(BitmapID::Strawberry, "Resources/Images/fresa.png");
}

void Game::renderMap(const charMatrix &map)
{
    for(int row = 0; row < int(map.size()); ++row)
    {
        for(int col = 0; col < int(map[row].size()); ++col)
        {
            if(map[row][col] == 'X'){
               al_draw_bitmap(mBitmaps.get(BitmapID::Wall).get(),
                              col * mBitmaps.get(BitmapID::Wall).width(),
                              row * mBitmaps.get(BitmapID::Wall).height(),
                              0);

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
        ALLEGRO_BITMAP* bitmap;

        if(type == Enemy::Type::RED)
        {
            bitmap = mBitmaps.get(BitmapID::RedGhost).get();
        }
        else if(type == Enemy::Type::GREEN)
        {
            bitmap = mBitmaps.get(BitmapID::GreenGhost).get();
        }

        auto enemy = std::make_unique<Enemy>(randX, randY, Direction::RIGHT, mMap,
                                             bitmap, type);
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
        ALLEGRO_BITMAP* bitmap;
        switch(rnd)
        {
            case 0:
                randType = Fruit::Type::FRESA;
                bitmap = mBitmaps.get(BitmapID::Strawberry).get();
                break;
            case 1:
                randType = Fruit::Type::HONGO;
                bitmap = mBitmaps.get(BitmapID::Mushroom).get();
                break;
            case 2:
                randType = Fruit::Type::MANSANA;
                bitmap = mBitmaps.get(BitmapID::Apple).get();
                break;
            case 3:
                randType = Fruit::Type::NARANJA;
                bitmap = mBitmaps.get(BitmapID::Orange).get();
                break;
            default:
                randType = Fruit::Type::PERA;
                bitmap = mBitmaps.get(BitmapID::Pear).get();
                break;

        }
        auto fruit = std::make_unique<Fruit>(randX, randY, randType, bitmap);
        mFruits.push_back(std::move(fruit));
    }
}

void Game::togglePause()
{
    //Using space key we can pause or resume our game
    if(mGameState == GameState::PLAYING)
    {
        mGameState = GameState::PAUSE;
        stopAllTimers();
        al_stop_sample_instance(mBackgroundMusicInstance.get());
    }
    else if(mGameState == GameState::PAUSE)
    {
        mGameState = GameState::PLAYING;
        startAllTimers();
        al_play_sample_instance(mBackgroundMusicInstance.get());
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
    mTimer.start();
    mFantomTimer.start();
    mFrameTimer.start();
}

void Game::stopAllTimers()
{
    mTimer.stop();
    mFantomTimer.stop();
    mFrameTimer.stop();
}

void Game::checkPacmanCollisions()
{
    //Checking for pacman and food collisions
    for(auto &f : mFruits)
    {
        if(!f->isEaten() && mPacman->isCollision(f->getX(), f->getY()))
        {
            mPacman->score(f.get());
            al_play_sample(mSounds.get(SampleID::PacmanChomp).get(),
                           1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
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
    al_play_sample(mSounds.get(SampleID::PagmanDeath).get(),
                   1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
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

void Game::startNewGame()
{
    mPacman->reset();
    mFruits.clear();
    mEnemies.clear();
    createFruits();
    createEnemies();
    startAllTimers();
    mGameState = GameState::PLAYING;
}

//For debugging only
void Game::printMap()
{
    for(auto &line: mMap)
    {
        std::cout << line << std::endl;
    }

}

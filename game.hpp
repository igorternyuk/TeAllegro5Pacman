#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <vector>
#include <map>
#include "pacman.hpp"
#include "enemy.hpp"
#include "fruit.hpp"

class Game
{
public:
    explicit Game();
    ~Game();
    void run();
private:
    enum
    {
        SCREEN_WIDTH  = 928,
        SCREEN_HEIGHT  = 690,
        WINDOW_LEFT = 200,
        WINDOW_TOP = 20,
        TILE_SIZE = 32
    };

    const char* WINDOW_TITLE = "Pacman";
    const std::string GAME_PAUSED_TEXT = "GAME PAUSED";
    const std::string WIN_MESSAGE_TEXT = "CONGRATULATIONS!!!YOU WON!!!";
    const std::string LOST_MESSAGE_TEXT = "YOU LOST!!!";
    const std::string PATH_TO_SETTINGS_FILE = "settings.txt";

    ALLEGRO_DISPLAY *mDisplay;
    ALLEGRO_BITMAP *mWallBitmap = nullptr;
    ALLEGRO_BITMAP* mPacmanBitmap;
    std::map<Enemy::Type, ALLEGRO_BITMAP*> mEnemyBitmaps;
    std::map<Fruit::Type, ALLEGRO_BITMAP*> mFruitBitmaps;

    //Sounds
    ALLEGRO_SAMPLE *mSoundPacmanChomp;
    ALLEGRO_SAMPLE *mSoundPacmanDeath;
    ALLEGRO_SAMPLE *mBackgroundMusicSample;

    // Background music
    ALLEGRO_SAMPLE_INSTANCE *mBackgroundMusicInstance;

    //Fonts
    ALLEGRO_FONT *mSmallFont;
    ALLEGRO_FONT *mMiddleFont;
    ALLEGRO_FONT *mLargeFont;

    //Timers
    ALLEGRO_TIMER *mTimer;
    ALLEGRO_TIMER *mFrameTimer;
    ALLEGRO_TIMER *mFantomTimer;

    //Keyboard
    ALLEGRO_KEYBOARD_STATE mKeyState;
    ALLEGRO_EVENT_QUEUE *mEventQueue;

    charMatrix mMap;
    int mInitPacmanX, mInitPacmanY, mPacmanLivesCount, mEnemiesCount;
    int mFoodAmount;
    bool mDone = false;
    bool mRender = false;
    //Probably these three booleans should be replaced with enum GameStatus
    bool mIsGamePaused = false;
    bool mIsWin = false;
    bool mIsLost = false;
    const float mPacMovingSpeed = 4.0f;    // Скорость движения пакмена
    const float mFantomMovingSpeed = 1.0f; // Скорость фантомов
    const float mPacAnimSpeed = 10.0f;     // Скорость анимации пакмена
    Pacman *mPacman;
    std::vector<Fruit*> mFruits;
    std::vector<Enemy*> mEnemies;

    //////////////////
    void handleEvents();
    void update();
    void render();
    ////////////////
    void loadSettings(const std::string &fileName);
    void loadFonts();
    void loadSounds();
    void loadBitmaps();
    void destroyBitmaps();
    void destroyFonts();
    void destroySounds();
    void createEnemies();
    void createFruits();
    void handlePacmanEnemyCollision(Enemy *&enemy);
    void drawMap(ALLEGRO_BITMAP *mWallBitmap, const charMatrix &mMap);
    void drawPacmanScore(int left, int top, ALLEGRO_FONT *mSmallFont);
    void drawPacmanLives(ALLEGRO_FONT *mSmallFont);
    void drawMessage(const int &textLeft, const int &textTop,
                     const std::string &text, ALLEGRO_COLOR color,
                     ALLEGRO_FONT *mSmallFont);
    void prepareNewGame();
    void printMap();
};

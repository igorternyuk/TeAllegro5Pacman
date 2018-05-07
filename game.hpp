#pragma once

#include "allegro5initializer.hpp"

#include "pacman.hpp"
#include "enemy.hpp"
#include "fruit.hpp"
#include "resourceindentifiers.hpp"
#include "resourcemanager.hpp"
#include "allegro5bitmap.hpp"
#include "allegro5font.hpp"
#include "allegro5sample.hpp"
#include "allegro5timer.hpp"

#include <vector>
#include <map>
#include <memory>

class Game
{
public:
    using BitmapManager = ResourceManager<Allegro5Bitmap, BitmapID>;
    using FontManager = ResourceManager<Allegro5Font, FontID>;
    using SoundManager = ResourceManager<Allegro5Sample, SampleID>;
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

    enum class GameState
    {
        PLAYING,
        PAUSE,
        VICTORY,
        DEFEAT
    };

    const char* WINDOW_TITLE = "Pacman";
    const char*GAME_PAUSED_TEXT = "GAME PAUSED";
    const char* WIN_MESSAGE_TEXT = "CONGRATULATIONS!!!YOU WON!!!";
    const char* LOST_MESSAGE_TEXT = "YOU LOST!!!";
    const std::string PATH_TO_SETTINGS_FILE = "settings.txt";

    Allegro5Initializer mAllegro5Initializer;
    ALLEGRO_DISPLAY *mDisplay;
    BitmapManager mBitmaps;
    FontManager mFonts;
    SoundManager mSounds;
    std::string mPathToWallBitmap;

    // Background music
    ALLEGRO_SAMPLE_INSTANCE *mBackgroundMusicInstance;

    //Timers
    my_unique_ptr<ALLEGRO_TIMER> mTimer;
    my_unique_ptr<ALLEGRO_TIMER> mFrameTimer;
    my_unique_ptr<ALLEGRO_TIMER> mFantomTimer;

    //Keyboard
    ALLEGRO_KEYBOARD_STATE mKeyState;
    ALLEGRO_EVENT_QUEUE *mEventQueue;

    charMatrix mMap;
    int mInitPacmanX, mInitPacmanY, mPacmanLivesCount, mEnemiesCount;
    int mFoodAmount;
    bool mIsRunning = true;
    bool mRender = false;
    const float mPacMovingSpeed = 4.0f;
    const float mFantomMovingSpeed = 1.0f;
    const float mPacAnimSpeed = 10.0f;
    std::unique_ptr<Pacman> mPacman;
    std::vector<std::unique_ptr<Fruit>> mFruits;
    std::vector<std::unique_ptr<Enemy>> mEnemies;
    GameState mGameState = GameState::PLAYING;

    void update();
    void render();
    void loadSettings(const std::string &fileName);
    void loadFonts();
    void loadSounds();
    void loadBitmaps();
    void destroyBitmaps();
    void destroyFonts();
    void destroySounds();
    void createEnemies();
    void createFruits();
    void togglePause();
    void checkGameState();
    void startAllTimers();
    void stopAllTimers();
    void checkPacmanCollisions();
    void checkFantomCollisions();
    void handlePacmanEnemyCollision(Enemy &enemy);
    void renderMap(const charMatrix &mMap);
    void renderPacmanScore(int left, int top, ALLEGRO_FONT *mSmallFont);
    void renderPacmanLives(ALLEGRO_FONT *mSmallFont);
    void renderTextMessage(const int &textLeft, const int &textTop,
                     const std::string &text, ALLEGRO_COLOR color,
                     ALLEGRO_FONT *mSmallFont);
    void prepareNewGame();
    void printMap();
};

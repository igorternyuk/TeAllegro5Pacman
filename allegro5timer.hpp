#pragma once
#include <allegro5/allegro.h>
#include "my_untils.hpp"

class Allegro5Timer
{
public:
    explicit Allegro5Timer(double speedSecs);
    void start();
    void stop();
    void setTimerSpeed(double speed);
    double getTimerSpeed() const;
    void setTimerCount(int64_t count);
    int64_t getTimerCount() const;
    bool isActive() const;
    ALLEGRO_TIMER* get() const;
    ALLEGRO_EVENT_SOURCE* getEventSource() const;
private:
    my_unique_ptr<ALLEGRO_TIMER> mTimer;
};

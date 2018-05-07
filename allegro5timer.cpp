#include "allegro5timer.hpp"
#include <stdexcept>

Allegro5Timer::Allegro5Timer(double speedSecs)
{
    auto timer = al_create_timer(speedSecs);
    if(!timer)
    {
        throw std::runtime_error("Could not create timer");
    }
    my_unique_ptr<ALLEGRO_TIMER> ptr{ timer, al_destroy_timer };
    mTimer.swap(ptr);
}

void Allegro5Timer::start()
{
    al_start_timer(mTimer.get());
}

void Allegro5Timer::stop()
{
    al_stop_timer(mTimer.get());
}

void Allegro5Timer::setTimerSpeed(double speed)
{
    al_set_timer_speed(mTimer.get(), speed);
}

double Allegro5Timer::getTimerSpeed() const
{
    return al_get_timer_speed(mTimer.get());
}

void Allegro5Timer::setTimerCount( int64_t count)
{
    al_set_timer_count(mTimer.get(), count);
}

int64_t Allegro5Timer::getTimerCount() const
{
    return al_get_timer_count(mTimer.get());
}

bool Allegro5Timer::isActive() const
{
    return al_get_timer_started(mTimer.get());
}

ALLEGRO_TIMER *Allegro5Timer::get() const
{
    return mTimer.get();
}

ALLEGRO_EVENT_SOURCE *Allegro5Timer::getEventSource() const
{
    return al_get_timer_event_source(mTimer.get());
}

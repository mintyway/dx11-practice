#include "Timer.h"

Timer::Timer() : pausedDuration(Duration<double>::zero()), deltaSeconds(0.0f), isReset(true), isStopped(true) {}

void Timer::reset()
{
    baseTime = TimePoint{};
    previousTime = TimePoint{};
    stopTime = TimePoint{};
    pausedDuration = Duration<double>::zero();
    deltaSeconds = 0.0f;
    isReset = true;
    isStopped = true;
}

void Timer::start()
{
    const TimePoint currentTime = Clock::now();

    if (isReset)
    {
        baseTime = currentTime;
        isReset = false;
    }

    if (isStopped)
    {
        pausedDuration += currentTime - stopTime;
        previousTime = currentTime;
        isStopped = false;
    }
}

void Timer::stop()
{
    if (!isStopped)
    {
        stopTime = Clock::now();
        isStopped = true;
    }
}

void Timer::Tick()
{
    if (isStopped)
    {
        deltaSeconds = 0.0f;
        return;
    }

    const TimePoint currentTime = Clock::now();
    deltaSeconds = Duration<float>(currentTime - previousTime).count();
    previousTime = currentTime;

    deltaSeconds = std::max(0.0f, deltaSeconds);
}

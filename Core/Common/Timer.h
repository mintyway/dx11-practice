#pragma once

#include <chrono>

class Timer
{
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    template <typename T>
    using Duration = std::chrono::duration<T>;

    Timer();

    [[nodiscard]] float GetDeltaSeconds() const { return deltaSeconds; }
    [[nodiscard]] double GetTotalSeconds() const { return isStopped ? GetElapsedSeconds(stopTime) : GetElapsedSeconds(Clock::now()); }

    void Reset();
    void Start();
    void Stop();
    void Tick();

private:
    [[nodiscard]] double GetElapsedSeconds(const TimePoint& currentTime) const { return Duration<double>(currentTime - baseTime - pausedDuration).count(); }

    TimePoint baseTime;
    TimePoint previousTime;
    TimePoint stopTime;
    Duration<double> pausedDuration;

    float deltaSeconds;

    bool isReset;
    bool isStopped;
};

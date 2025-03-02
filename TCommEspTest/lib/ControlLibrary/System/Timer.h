#ifndef TIMER_H
#define TIMER_H

#if defined(_WIN32) || defined(_WIN64)
    #include <chrono>
#elif defined(__linux__)
    #include <ctime>
#else
    #include <Arduino.h>
#endif
namespace GenericLibrary
{
class Timer
{
private:
    bool P_startTimer;
    long setTime; // Time duration in microseconds
#if defined(_WIN32) || defined(_WIN64)
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
#elif defined(__linux__)
    struct timespec startTime;
    struct timespec currentTime;
#else
    long startTime;
    long currentTime;
#endif
    bool startTimer;
    bool timerDone;
    bool useMicros;

public:
    Timer(uint16_t inputTime)
    {
        setTime = static_cast<long>(inputTime) * 1000; // Default to milliseconds
        startTimer = false;
        useMicros = false;
    }

    void SetTime(uint16_t inputTime)
    {
        if (useMicros)
        {
            setTime = static_cast<long>(inputTime);
        }
        else
        {
            setTime = static_cast<long>(inputTime) * 1000; // Convert milliseconds to microseconds
        }
    }

    void SetMicros()
    {
        useMicros = true;
    }

    void SetMillis()
    {
        useMicros = false;
    }

    void Start()
    {
        startTimer = true;
    }

    void Stop()
    {
        startTimer = false;
    }

    void Run()
    {
        int64_t elapsedTime;
        if (startTimer)
        {
#if defined(_WIN32) || defined(_WIN64)
            currentTime = std::chrono::steady_clock::now();
            if (!P_startTimer)
            {
                startTime = currentTime;
            }
            elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime).count();
#elif defined(__linux__)
            clock_gettime(CLOCK_MONOTONIC, &currentTime);
            if (!P_startTimer)
            {
                startTime = currentTime;
            }
            elapsedTime = (currentTime.tv_sec - startTime.tv_sec) * 1000000LL + // Seconds to microseconds
                        (currentTime.tv_nsec - startTime.tv_nsec) / 1000LL;   // Nanoseconds to microseconds
#else
            if (useMicros)
            {
                currentTime = micros();
            }
            else
            {
                currentTime = millis() * 1000; // Convert milliseconds to microseconds
            }
            if (!P_startTimer)
            {
                startTime = currentTime;
            }
            elapsedTime = currentTime - startTime;
#endif
        }
        timerDone = startTimer && (elapsedTime > setTime);
        P_startTimer = startTimer;
    }

    void Loop()
    {
        if (!Started())
        {
            Start();
        }
        else if (Done())
        {
            Stop();
        }
        Run();
    }

    bool Started() const
    {
        return startTimer;
    }

    bool Done() const
    {
        return timerDone;
    }
};
};
#endif // TIMER_H
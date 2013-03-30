#ifndef STOPWATCH_H
#define STOPWATCH_H
#include <chrono>

using namespace std::chrono;
typedef std::chrono::milliseconds milliseconds;

class StopWatch
{
public:
    StopWatch();
    void Start();
    StopWatch& Stop();
    int ElapsedMilliseconds();

private:
    high_resolution_clock::time_point begin;
    high_resolution_clock::time_point end;
};

#endif // STOPWATCH_H

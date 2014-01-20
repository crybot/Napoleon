#include "stopwatch.h"

namespace Napoleon
{
    StopWatch::StopWatch() :begin(t_clock::now())
    { }

    void StopWatch::Restart()
    {
        begin = t_clock::now();
    }

    double StopWatch::ElapsedMilliseconds()
    {
        return duration_cast<MS>(t_clock::now() - begin).count();
    }

    double StopWatch::ElapsedSeconds()
    {
        return duration_cast<std::chrono::seconds>(t_clock::now() - begin).count();
    }

    StopWatch StopWatch::StartNew()
    {
        StopWatch watch;
        watch.Restart();

        return watch;
    }
}

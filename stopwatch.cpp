#include "stopwatch.h"

namespace Napoleon
{
    StopWatch::StopWatch() { }

    void StopWatch::Start()
    {
        begin = high_resolution_clock::now();
    }

    StopWatch& StopWatch::Stop()
    {
        end = high_resolution_clock::now();
        return *this;
    }

    double StopWatch::ElapsedMilliseconds()
    {
        return duration_cast<MS>(end - begin).count();
    }

    double StopWatch::ElapsedSeconds()
    {
        return duration_cast<MS>(end - begin).count() / static_cast<double>(1000);
    }

    StopWatch StopWatch::StartNew()
    {
        StopWatch watch;
        watch.Start();

        return watch;
    }
}

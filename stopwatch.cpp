#include "stopwatch.h"


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
    return duration_cast<MS>(end - begin).count() / (double)1000;

}

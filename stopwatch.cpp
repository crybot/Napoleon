#include "stopwatch.h"


StopWatch::StopWatch()
{

}

void StopWatch::Start()
{
    begin = high_resolution_clock::now();
}

StopWatch& StopWatch::Stop()
{
    end = high_resolution_clock::now();
    return *this;
}

int StopWatch::ElapsedMilliseconds()
{
    return duration_cast<milliseconds>(end - begin).count();
}

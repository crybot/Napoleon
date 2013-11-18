#ifndef STOPWATCH_H
#define STOPWATCH_H
#include <chrono>

namespace Napoleon
{
    using namespace std::chrono;
    typedef std::chrono::milliseconds MS;

    class StopWatch
    {
    public:
        StopWatch();
        StopWatch& Stop();
        void Start();
        double ElapsedMilliseconds();
        double ElapsedSeconds();

        static StopWatch StartNew();

    private:
        high_resolution_clock::time_point begin;
        high_resolution_clock::time_point end;
    };
}

#endif // STOPWATCH_H

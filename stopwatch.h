#ifndef STOPWATCH_H
#define STOPWATCH_H
#include <chrono>

namespace Napoleon
{
    using namespace std::chrono;
    typedef std::chrono::milliseconds MS;
    typedef std::chrono::steady_clock t_clock;

    class StopWatch
    {
    public:
        StopWatch();
        void Restart();
        double ElapsedMilliseconds();
        double ElapsedSeconds();

        static StopWatch StartNew();

    private:
        t_clock::time_point begin;
        t_clock::time_point end;
    };
}

#endif // STOPWATCH_H

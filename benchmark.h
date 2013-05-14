#ifndef BENCHMARK_H
#define BENCHMARK_H
#include "board.h"

namespace Napoleon
{
    class Benchmark
    {
    public:
        Benchmark();
        void Start();
        void CutoffTest();

        unsigned long long Perft(int, Board&);
    private:
        Board board;
    };
}

#endif // BENCHMARK_H

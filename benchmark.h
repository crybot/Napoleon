#ifndef BENCHMARK_H
#define BENCHMARK_H

namespace Napoleon
{
    class Board;
    class Benchmark
    {
    public:
        Benchmark(Board&);
        void Start(int);
        void CutoffTest();

        unsigned long long Perft(int);
    private:
        Board& board;
    };
}

#endif // BENCHMARK_H

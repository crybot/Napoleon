#ifndef SEARCH_H
#define SEARCH_H
#include "defines.h"
#include "move.h"
#include "constants.h"
#include "searchinfo.h"
#include "parallelinfo.h"
#include <cstring>
#include <condition_variable>
#include <vector>
#include <thread>
#include <atomic>

namespace Napoleon
{
    enum class SearchType
    {
        Infinite, TimePerGame, TimePerMove, Ponder
    };

    enum class NodeType
    {
        PV, NONPV, CUT, ALL
    };

    class Board;
    class TranspositionTable;
    namespace Search
    {
        extern const int AspirationValue;
        extern bool pondering;
        extern std::atomic<bool> PonderHit;
        extern std::atomic<bool> StopSignal;
        extern int MoveTime;
        extern int GameTime[2]; // by color
        extern thread_local SearchInfo searchInfo;
        extern thread_local bool sendOutput;
        extern TranspositionTable Table;
        extern std::condition_variable parallel;
        extern ParallelInfo parallelInfo;
        extern std::vector<std::thread> threads;
        extern int depth_limit;
        extern int cores;
        extern std::atomic<bool> quit;
        extern const int default_cores;

        void InitializeThreads(int = default_cores);
        void KillThreads();
        void signalThreads(int, int, int, const Board&, bool);
        void parallelSearch();
        int predictTime(Color);

        std::string GetInfo(Board&, Move, int, int, int);
        std::string GetPv(Board&, Move, int);
        Move getPonderMove(Board&, const Move);

        Move StartThinking(SearchType, Board&, bool=true, bool=false);
        void StopThinking();
        Move iterativeSearch(Board&);
        int searchRoot(int, int, int, Move&, Board&);

        template<NodeType>
        int search(int, int, int, int, Board&, bool);
        int quiescence(int, int, Board&);

        int razorMargin(int);
    }

    inline int Search::razorMargin(int depth)
    {
        return (25*(depth-1) + 150);
    }
}

#endif // SEARCH_H

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
#include <fstream>

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
        extern std::ofstream* positions_dataset;
        extern bool record_positions;

        enum Parameters : int {
            RAZOR1=0, RAZOR2, RAZOR3, 
            REVERSENULL_DEPTH, REVERSENULL1, REVERSENULL2,
            NULLPRUNE1, NULLPRUNE2, NULLPRUNE3,
            FUTILITY1, FUTILITY2,
            IID_DEPTH, IID1,
            LMR_DEPTH1, LMR_DEPTH2, LMR_DEPTH3,
            LMR_R1, LMR_R2,
            LMR_MARGIN,
            MAX
        };
        extern int param[];
        extern std::string param_name[];

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
        int searchRoot(int, int, int, Move&, Board&, const Move=Constants::NullMove);

        template<NodeType>
            int search(int, int, int, int, Board&, bool);
        int quiescence(int, int, Board&);

        int razorMargin(int);
    }

    inline int Search::razorMargin(int depth)
    {
        return (param[RAZOR1]*(depth-param[RAZOR2]) + param[RAZOR3]);
    }
}

#endif // SEARCH_H

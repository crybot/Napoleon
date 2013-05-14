#ifndef SEARCH_H
#define SEARCH_H
#include "defines.h"
#include "board.h"
#include "move.h"
#include "constants.h"
#include "movegenerator.h"
#include "constants.h"
#include "stopwatch.h"
#include "console.h"
#include <cstring>

namespace Napoleon
{
    class Board;
    class Move;
    namespace Search
    {

        extern StopWatch Timer;
        extern int ThinkTime;
        extern int moveScores[Constants::MaxMoves];
        extern int history[2][64][64];
        extern int lastScore;
        extern Move killerMoves[Constants::MaxPly][2];

        void IterativeSearch(Board&);
        int searchRoot(int, int, int, Move&, Board&);

        template<int>
        int search(int, int, int, Board&);
        int quiescence(int, int, Board&);

        void setScores(Move[], Board&, int, int);
        void pickMove(Move[], int, int);
        void orderCaptures(Move[], Board&, int, int);
    }

}

#endif // SEARCH_H

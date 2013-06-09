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
    enum SearchTask { Think, Infinite, Stop, Quit };
    static const int rMargin[] = { 0, Constants::Piece::PieceValue[PieceType::Knight],  Constants::Piece::PieceValue[PieceType::Rook] };

    class Board;
    namespace Search
    {
        extern const int AspirationValue;
        extern bool MoveTime;
        extern SearchTask Task;
        extern StopWatch Timer;
        extern int ThinkTime;
        extern int Time[2];
        extern int moveScores[Constants::MaxMoves];
        extern int history[2][4096];
        extern int nodes;
        extern Move killerMoves[Constants::MaxPly][2];

        std::string GetInfo(Board&, Move, int, int, int);
        std::string GetPv(Board&, Move, int);

        void StartThinking(Board&);
        void StopThinking();
        Move iterativeSearch(Board&);
        int searchRoot(int, int, int, Move&, Board&);

        int search(int, int, int, Board&);
        int quiescence(int, int, Board&);

        void setScores(Move[], Board&, int, int);
        void pickMove(Move[], int, int);
        void orderCaptures(Move[], Board&, int, int);

        int futilityMargin(int);
        int razorMargin(int);
    }

    inline int Search::futilityMargin(int depth)
    {
        return rMargin[depth];
    }
    inline int Search::razorMargin(int depth)
    {
        return (50 + 25*(depth-1));
    }
}

#endif // SEARCH_H

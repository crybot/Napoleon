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
#include "searchinfo.h"
#include <cstring>

namespace Napoleon
{
    static const int rMargin[] = { 0, Constants::Piece::PieceValue[PieceType::Knight],  Constants::Piece::PieceValue[PieceType::Rook] };

    enum class SearchType
    {
        Infinite, TimePerGame, TimePerMove
    };

    class Board;
    namespace Search
    {
        extern const int AspirationValue;
        extern bool StopSignal;
        extern int MoveTime;
        extern int GameTime[2]; // by color
        extern SearchInfo searchInfo;
        extern bool sendOutput;

        std::string GetInfo(Board&, Move, int, int, int);
        std::string GetPv(Board&, Move, int);

        Move StartThinking(SearchType, Board&, bool=true, bool=false);
        void StopThinking();
        Move iterativeSearch(Board&);
        int searchRoot(int, int, int, Move&, Board&);

        int search(int, int, int, int, Board&);
        int quiescence(int, int, Board&);

        //        int futilityMargin(int);
        //        int razorMargin(int);
    }

    //    inline int Search::futilityMargin(int depth)
    //    {
    //        return rMargin[depth];
    //    }
    //    inline int Search::razorMargin(int depth)
    //    {
    //        return (50 + 25*(depth-1));
    //    }
}

#endif // SEARCH_H

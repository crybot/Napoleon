#ifndef QUEEN_H
#define QUEEN_H
#include "defines.h"

namespace Napoleon
{
    class Board;
    class Queen
    {
    public:
        static BitBoard GetAllTargets(BitBoard queens, Board& board);
    };



}

#endif // QUEEN_H

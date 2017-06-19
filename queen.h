#ifndef QUEEN_H
#define QUEEN_H
#include "defines.h"

namespace Napoleon
{
    class Board;
    class Queen
    {
    public:
        static BitBoard GetAllTargets(BitBoard, Board&);
        static BitBoard TargetsFrom(Square, Color, Board&);
    };
}

#endif // QUEEN_H

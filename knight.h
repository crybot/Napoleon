#ifndef KNIGHT_H
#define KNIGHT_H
#include "defines.h"

namespace Napoleon
{
    class Board;
    class Knight
    {
    public:
        static BitBoard GetAllTargets(BitBoard, Board&);
        static BitBoard GetKnightAttacks(BitBoard);

    };
}

#endif // KNIGHT_H

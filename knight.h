#ifndef KNIGHT_H
#define KNIGHT_H
#include "defines.h"
#include "board.h"

namespace Napoleon
{
    class Knight
    {
    public:
        static BitBoard GetAllTargets(BitBoard, Board&);
        static BitBoard GetKnightAttacks(BitBoard);

    };
}

#endif // KNIGHT_H

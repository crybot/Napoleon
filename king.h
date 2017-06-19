#ifndef KING_H
#define KING_H
#include "defines.h"

namespace Napoleon
{
    class Board;
    class King
    {
    public:
        static BitBoard GetAllTargets(BitBoard, Board&);
        static BitBoard GetKingAttacks(BitBoard);

    };
}

#endif // KING_H

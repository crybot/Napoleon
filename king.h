#ifndef KING_H
#define KING_H
#include "defines.h"
#include "board.h"

namespace Napoleon
{
    class King
    {

    public:
        static BitBoard GetAllTargets(BitBoard, Board&);
        static BitBoard GetKingAttacks(BitBoard);

    };
}

#endif // KING_H

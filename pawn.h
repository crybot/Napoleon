#ifndef PAWN_H
#define PAWN_H
#include "defines.h"
#include "board.h"

namespace Napoleon
{
    class Pawn
    {

    public:

        static BitBoard GetAllTargets(Byte color, BitBoard pawns, Board& board);
        __always_inline static BitBoard GetAnyAttack(Byte color, BitBoard pawns, Board& board);

    private:
        __always_inline static BitBoard GetQuietTargets(Byte color, BitBoard pawns, BitBoard empty);
        __always_inline static BitBoard GetSinglePushTargets(Byte color, BitBoard pawns, BitBoard empty);
        __always_inline static BitBoard GetDoublePushTargets(Byte color, BitBoard pawns, BitBoard empty);
        __always_inline static BitBoard GetPawnsAbleToSinglePush(Byte color, BitBoard pawns, BitBoard empty);
        __always_inline static BitBoard GetPawnsAbleToDoublePush(Byte color, BitBoard pawns, BitBoard empty);
        __always_inline static BitBoard GetEastAttacks(Byte color, BitBoard pawns);
        __always_inline static BitBoard GetWestAttacks(Byte color, BitBoard pawns);

    };

}

#endif // PAWN_H

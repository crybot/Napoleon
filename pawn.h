#ifndef PAWN_H
#define PAWN_H
#include "defines.h"
#include "board.h"

namespace Napoleon
{
    class Pawn
    {
    public:

        static BitBoard GetAllTargets(BitBoard pawns, Board& board);
        INLINE static BitBoard GetAnyAttack(BitBoard pawns, Board& board);

    private:
        INLINE static BitBoard GetQuietTargets(Byte color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetSinglePushTargets(Byte color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetDoublePushTargets(Byte color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetPawnsAbleToSinglePush(Byte color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetPawnsAbleToDoublePush(Byte color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetEastAttacks(Byte color, BitBoard pawns);
        INLINE static BitBoard GetWestAttacks(Byte color, BitBoard pawns);

    };
}

#endif // PAWN_H

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
        INLINE static BitBoard GetQuietTargets(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetSinglePushTargets(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetDoublePushTargets(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetPawnsAbleToSinglePush(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetPawnsAbleToDoublePush(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetEastAttacks(Color color, BitBoard pawns);
        INLINE static BitBoard GetWestAttacks(Color color, BitBoard pawns);

    };
}

#endif // PAWN_H

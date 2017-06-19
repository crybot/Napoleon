#ifndef PAWN_H
#define PAWN_H
#include "defines.h"
#include "compassrose.h"

namespace Napoleon
{
    class Board;
    class Pawn
    {
    public:

        static BitBoard GetAllTargets(BitBoard pawns, Board& board);
        INLINE static BitBoard GetAnyAttack(BitBoard pawns, Board& board);
		static BitBoard GetAnyAttack(BitBoard pawns, Color color, BitBoard squares);
        static BitBoard GetQuietTargets(Color color, BitBoard pawns, BitBoard empty);

    private:
        static BitBoard GetSinglePushTargets(Color color, BitBoard pawns, BitBoard empty);
        static BitBoard GetDoublePushTargets(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetPawnsAbleToSinglePush(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetPawnsAbleToDoublePush(Color color, BitBoard pawns, BitBoard empty);
        INLINE static BitBoard GetEastAttacks(Color color, BitBoard pawns);
        INLINE static BitBoard GetWestAttacks(Color color, BitBoard pawns);

    };

    INLINE BitBoard Pawn::GetQuietTargets(Color color, BitBoard pawns, BitBoard empty)
    {
        return GetSinglePushTargets(color, pawns, empty) | GetDoublePushTargets(color, pawns, empty);
    }

    INLINE BitBoard Pawn::GetSinglePushTargets(Color color, BitBoard pawns, BitBoard empty)
    {
        return color == PieceColor::White ? CompassRose::OneStepNorth(pawns) & empty : CompassRose::OneStepSouth(pawns) & empty;
    }

    INLINE BitBoard Pawn::GetDoublePushTargets(Color color, BitBoard pawns, BitBoard empty)
    {
        BitBoard singlePush = GetSinglePushTargets(color, pawns, empty);

        return color == PieceColor::White
                ? CompassRose::OneStepNorth(singlePush) & empty & Constants::Ranks::Four
                : CompassRose::OneStepSouth(singlePush) & empty & Constants::Ranks::Five;
    }

}

#endif // PAWN_H

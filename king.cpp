#include "king.h"
#include "utils.h"
#include "movedatabase.h"
#include "compassrose.h"
#include "board.h"

namespace Napoleon
{
    BitBoard King::GetAllTargets(BitBoard king, Board& board)
    {
        BitBoard kingMoves = MoveDatabase::KingAttacks[(Utils::BitBoard::BitScanForward(king))];
        return kingMoves & ~board.GetPlayerPieces();
    }

    BitBoard King::GetKingAttacks(BitBoard king)
    {
        BitBoard attacks = CompassRose::OneStepEast(king) | CompassRose::OneStepWest(king);
        king |= attacks;
        attacks |= CompassRose::OneStepNorth(king) | CompassRose::OneStepSouth(king);
        return attacks;
    }
}

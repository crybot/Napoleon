#include "knight.h"
#include "compassrose.h"
#include "movedatabase.h"
#include "utils.h"

namespace Napoleon
{
    BitBoard Knight::GetAllTargets(BitBoard knights, Board& board)
    {
        BitBoard targets = MoveDatabase::KnightAttacks[(Utils::BitBoard::BitScanForward(knights))];

        return targets & ~board.GetPlayerPieces();
    }

    BitBoard Knight::GetKnightAttacks(BitBoard knights)
    {
        BitBoard west, east, attacks;
        east = CompassRose::OneStepEast(knights);
        west = CompassRose::OneStepWest(knights);
        attacks = (east | west) << 16;
        attacks |= (east | west) >> 16;
        east = CompassRose::OneStepEast(east);
        west = CompassRose::OneStepWest(west);
        attacks |= (east | west) << 8;
        attacks |= (east | west) >> 8;

        return attacks;
    }
}

#include "pawn.h"
#include "board.h"
#include "compassrose.h"

namespace Napoleon
{   
    BitBoard Pawn::GetAllTargets(Byte color, BitBoard pawns, Board& board)
    {
        BitBoard empty = board.EmptySquares;

        return GetQuietTargets(color, pawns, empty) | GetAnyAttack(color, pawns, board);
    }

    BitBoard Pawn::GetQuietTargets(Byte color, BitBoard pawns, BitBoard empty)
    {
        return GetSinglePushTargets(color, pawns, empty) | GetDoublePushTargets(color, pawns, empty);
    }

    BitBoard Pawn::GetSinglePushTargets(Byte color, BitBoard pawns, BitBoard empty)
    {
        return color == PieceColor::White ? CompassRose::OneStepNorth(pawns) & empty : CompassRose::OneStepSouth(pawns) & empty;
    }

    BitBoard Pawn::GetDoublePushTargets(Byte color, BitBoard pawns, BitBoard empty)
    {
        BitBoard singlePush = GetSinglePushTargets(color, pawns, empty);

        return color == PieceColor::White
                ? CompassRose::OneStepNorth(singlePush) & empty & Constants::Ranks::Four
                : CompassRose::OneStepSouth(singlePush) & empty & Constants::Ranks::Five;
    }

    BitBoard Pawn::GetPawnsAbleToSinglePush(Byte color, BitBoard pawns, BitBoard empty)
    {
        switch (color)
        {
        case PieceColor::White:
            return CompassRose::OneStepSouth(empty) & pawns;
        case PieceColor::Black:
            return CompassRose::OneStepNorth(empty) & pawns;
        default:
            throw std::exception();
        }
    }

    BitBoard Pawn::GetPawnsAbleToDoublePush(Byte color, BitBoard pawns, BitBoard empty)
    {
        switch (color)
        {
        case PieceColor::White:
        {
            BitBoard emptyRank3 = CompassRose::OneStepSouth(empty & Constants::Ranks::Four) & empty;
            return GetPawnsAbleToSinglePush(color, pawns, emptyRank3);
        }
        case PieceColor::Black:
        {
            BitBoard emptyRank6 = CompassRose::OneStepNorth(empty & Constants::Ranks::Six) & empty;
            return GetPawnsAbleToSinglePush(color, pawns, emptyRank6);
        }
        default:
            throw std::exception();
        }
    }

    BitBoard Pawn::GetEastAttacks(Byte color, BitBoard pawns)
    {
        return color == PieceColor::White ? CompassRose::OneStepNorthEast(pawns) : CompassRose::OneStepSouthEast(pawns);
    }

    BitBoard Pawn::GetWestAttacks(Byte color, BitBoard pawns)
    {
        return color == PieceColor::White ? CompassRose::OneStepNorthWest(pawns) : CompassRose::OneStepSouthWest(pawns);
    }

    BitBoard Pawn::GetAnyAttack(Byte color, BitBoard pawns, Board& board)
    {
        return (GetEastAttacks(color, pawns) | GetWestAttacks(color, pawns)) & board.GetEnemyPieces();
    }


}

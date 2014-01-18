#include "pawn.h"
#include "board.h"

namespace Napoleon
{   
    BitBoard Pawn::GetAllTargets(BitBoard pawns, Board& board)
    {
        BitBoard empty = board.EmptySquares;

        return GetQuietTargets(board.SideToMove(), pawns, empty) | GetAnyAttack(pawns, board);
    }

    BitBoard Pawn::GetAnyAttack(BitBoard pawns, Board& board)
    {
        return (GetEastAttacks(board.SideToMove(), pawns) | GetWestAttacks(board.SideToMove(), pawns)) & board.EnemyPieces();
    }


    BitBoard Pawn::GetPawnsAbleToSinglePush(Color color, BitBoard pawns, BitBoard empty)
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

    BitBoard Pawn::GetPawnsAbleToDoublePush(Color color, BitBoard pawns, BitBoard empty)
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

    BitBoard Pawn::GetEastAttacks(Color color, BitBoard pawns)
    {
        return color == PieceColor::White ? CompassRose::OneStepNorthEast(pawns) : CompassRose::OneStepSouthEast(pawns);
    }

    BitBoard Pawn::GetWestAttacks(Color color, BitBoard pawns)
    {
        return color == PieceColor::White ? CompassRose::OneStepNorthWest(pawns) : CompassRose::OneStepSouthWest(pawns);
    }

}

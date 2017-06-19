#ifndef BISHOP_H
#define BISHOP_H
#include "defines.h"
#include "board.h"
#include "utils.h"
#include "movedatabase.h"

namespace Napoleon
{
    class Bishop
    {
    public:
        static BitBoard GetAllTargets(BitBoard, Board&);
        static BitBoard TargetsFrom(Square, Color, Board&);
    };

    INLINE BitBoard Bishop::GetAllTargets(BitBoard bishops, Board& board)
    {
        BitBoard occupiedSquares = board.OccupiedSquares;
        BitBoard targets = Constants::Empty;

        Square square = Utils::BitBoard::BitScanForward(bishops);

        targets |= MoveDatabase::GetA1H8DiagonalAttacks(occupiedSquares, square);
        targets |= MoveDatabase::GetH1A8DiagonalAttacks(occupiedSquares, square);

        return targets & ~board.PlayerPieces();
    }

    INLINE BitBoard Bishop::TargetsFrom(Square square, Color color, Board& board)
    {
        BitBoard occupiedSquares = board.OccupiedSquares;
        BitBoard targets = Constants::Empty;

        targets |= MoveDatabase::GetA1H8DiagonalAttacks(occupiedSquares, square);
        targets |= MoveDatabase::GetH1A8DiagonalAttacks(occupiedSquares, square);

        return targets & ~board.Pieces(color);
    }
}

#endif // BISHOP_H

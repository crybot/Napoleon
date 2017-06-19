#ifndef ROOK_H
#define ROOK_H
#include "defines.h"
#include "board.h"
#include "utils.h"
#include "movedatabase.h"

namespace Napoleon
{
    class Rook
    {
    public:
        static BitBoard GetAllTargets(BitBoard, Board&);
        static BitBoard TargetsFrom(Square, Color, Board&);

    };

    INLINE BitBoard Rook::GetAllTargets(BitBoard rooks, Board& board)
    {
        BitBoard occupiedSquares = board.OccupiedSquares;
        BitBoard targets;

        Square  square = Utils::BitBoard::BitScanForward(rooks);
        targets = MoveDatabase::GetRookAttacks(occupiedSquares, square);

        return targets & ~board.PlayerPieces();
    }

    INLINE BitBoard Rook::TargetsFrom(Square square, Color color, Board& board)
    {
        BitBoard occupiedSquares = board.OccupiedSquares;
        BitBoard targets;

        targets = MoveDatabase::GetRookAttacks(occupiedSquares, square);

        return targets & ~board.Pieces(color);
    }
}

#endif // ROOK_H

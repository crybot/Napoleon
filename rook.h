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
        static BitBoard GetAllTargets(BitBoard, Board& board);

    };

    INLINE BitBoard Rook::GetAllTargets(BitBoard rooks, Board& board)
    {
        BitBoard occupiedSquares = board.OccupiedSquares;
        BitBoard targets = Constants::Empty;

        Square  square = Utils::BitBoard::BitScanForward(rooks);

        targets |= MoveDatabase::GetRankAttacks(occupiedSquares, square);
        targets |= MoveDatabase::GetFileAttacks(occupiedSquares, square);

        return targets & ~board.GetPlayerPieces();
    }
}

#endif // ROOK_H

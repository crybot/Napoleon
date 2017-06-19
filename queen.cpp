#include "queen.h"
#include "board.h"
#include "rook.h"
#include "bishop.h"

namespace Napoleon
{
    BitBoard Queen::GetAllTargets(BitBoard queens, Board& board)
    {
        return Rook::GetAllTargets(queens, board) | Bishop::GetAllTargets(queens, board);
    }

    BitBoard Queen::TargetsFrom(Square square, Color color, Board& board)
    {
        return Rook::TargetsFrom(square, color, board) | Bishop::TargetsFrom(square, color, board);
    }
}

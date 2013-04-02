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
}

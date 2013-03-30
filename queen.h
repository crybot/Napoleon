#ifndef QUEEN_H
#define QUEEN_H
#include "defines.h"
#include "board.h"
#include "utils.h"
#include "bishop.h"
#include "rook.h"

namespace Napoleon
{
    class Queen
    {
    public:
        static BitBoard GetAllTargets(BitBoard queens, Board& board);
    };

    __always_inline BitBoard Queen::GetAllTargets(BitBoard queens, Board& board)
    {
        return Rook::GetAllTargets(queens, board) | Bishop::GetAllTargets(queens, board);
    }
}

#endif // QUEEN_H

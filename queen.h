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


}

#endif // QUEEN_H

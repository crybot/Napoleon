#ifndef ZOBRIST_H
#define ZOBRIST_H
#include "defines.h"

namespace Napoleon
{
    namespace Zobrist
    {
        extern ZobristKey Piece[2][6][64];
        extern ZobristKey Castling[16];
        extern ZobristKey Enpassant[8];
        extern ZobristKey Color;

        void Init();
        ZobristKey random();
    }
}

#endif // ZOBRIST_H

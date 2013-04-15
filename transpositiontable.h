#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H
#include "defines.h"

namespace Napoleon
{
    namespace Zobrist
    {
        extern BitBoard Piece[2][6][64];
        extern BitBoard Castling[16];
        extern BitBoard Enpassant[8];
        extern BitBoard Color;

        void Init();
        BitBoard random();
    }

    class TranspositionTable
    {
    public:
        TranspositionTable();
    };

}

#endif // TRANSPOSITIONTABLE_H

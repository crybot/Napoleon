#include "hashentry.h"

namespace Napoleon
{
    HashEntry::HashEntry()
    {

    }

    HashEntry::HashEntry(ZobristKey hash, Byte depth, int score, Move bestMove, Byte bound)
    {
        Hash = hash;
        Depth = depth;
        Score = score;
        Bound = bound;
        BestMove = bestMove;
    }
}

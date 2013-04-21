#include "hashentry.h"

namespace Napoleon
{
    HashEntry::HashEntry()
    {

    }

    HashEntry::HashEntry(ZobristKey hash, int depth, int score, int bestMove, Byte bound)
    {
        Hash = hash;
        Depth = depth;
        Score = score;
        Bound = bound;
        BestMove = bestMove;
    }
}

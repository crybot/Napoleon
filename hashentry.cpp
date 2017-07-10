#include "hashentry.h"

namespace Napoleon
{
    HashEntry::HashEntry()  { }

    HashEntry::HashEntry(ZobristKey hash, Byte depth, Byte age, int score, Move bestMove, ScoreType bound)
    {
        Hash = hash;
        Depth = depth;
        Score = score;
        //Bound = (bound | (age << 2));
        Bound = bound;
        BestMove = bestMove;
    }
}

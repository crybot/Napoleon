#include "transpositiontable.h"
#include <ctime>
namespace Napoleon
{
    TranspositionTable::TranspositionTable(unsigned long size)
    {
        Size = size;
        Table = new HashEntry[size];
    }

    void TranspositionTable::Save(HashEntry entry)
    {
        HashEntry* hash = &Table[entry.Hash % Size];

        hash->Hash = entry.Hash;
        hash->Score = entry.Score;
        hash->Depth = entry.Depth;
        hash->Bound = entry.Bound;
    }

    int TranspositionTable::Probe(ZobristKey key, int depth, int alpha, int beta)
    {
        HashEntry* hash = &Table[key % Size];

        if (hash->Hash == key)
        {
            if (hash->Depth >= depth)
            {
                if (hash->Bound == Exact)
                    return hash->Score;
                if (hash->Bound == Alpha && hash->Score <= alpha)
                    return alpha;
                if (hash->Bound == Beta && hash->Score >= beta)
                    return beta;
            }
        }

        return TranspositionTable::Unknown;
    }
}

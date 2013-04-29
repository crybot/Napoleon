#include "transpositiontable.h"
#include <ctime>
namespace Napoleon
{
    TranspositionTable::TranspositionTable(unsigned long size)
    {
        Size = size;
        Table = new HashEntry[size];
    }

    void TranspositionTable::Save(ZobristKey key, Byte depth, int score, Move move, Byte bound)
    {
        HashEntry* hash = &Table[key % Size];

        if (depth >= hash->Depth) // it runs faster than depth > hash->Depth
        {
            hash->Hash = key;
            hash->Score = score;
            hash->Depth = depth;
            hash->Bound = bound;
            hash->BestMove = move;
        }
    }

    int TranspositionTable::Probe(ZobristKey key, Byte depth, int alpha, Move* move, int beta)
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

            *move = hash->BestMove; // get best move on this position
        }
        return TranspositionTable::Unknown;
    }
}

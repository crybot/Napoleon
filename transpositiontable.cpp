#include "transpositiontable.h"
#include "constants.h"
#include <ctime>
namespace Napoleon
{
    TranspositionTable::TranspositionTable(unsigned long size)
    {
        Size = size/(sizeof(HashEntry*)*BucketSize);

        Table = new HashEntry*[Size];
        for (unsigned i=0; i<Size; i++)
        {
            Table[i] = new HashEntry[BucketSize];
        }
    }

    void TranspositionTable::Save(ZobristKey key, Byte depth, int score, Move move, Byte bound)
    {
        HashEntry* hash;
        int min = Constants::MaxPly;

        for (int i=0; i<BucketSize; i++)
        {
            hash = &Table[key % Size][i];
            if (hash->Depth < min)
                min = hash->Depth;
        }

        hash = &Table[key % Size][min];

        hash->Hash = key;
        hash->Score = score;
        hash->Depth = depth;
        hash->Bound = bound;
        hash->BestMove = move;
    }

    int TranspositionTable::Probe(ZobristKey key, Byte depth, int alpha, Move* move, int beta)
    {
        HashEntry* hash = &Table[key % Size][0];

        for (int i=0; i<BucketSize; i++, hash++)
        {
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
        }
        return TranspositionTable::Unknown;
    }

    void TranspositionTable::Clear()
    {
        for (unsigned i=0 ; i<Size; i++)
        {
            for (int l=0; l<BucketSize; l++)
                Table[i][l].Hash = 0;
        }
    }

    Move TranspositionTable::GetPv(ZobristKey key)
    {
        HashEntry* hash = &Table[key % Size][0];

        for (int i=0; i<BucketSize; i++, hash++)
        {
            if (hash->Hash == key)
            {
                if (!MoveEncode::IsNull(hash->BestMove))
                    return hash->BestMove;
            }
        }

        return Constants::NullMove;
    }
}

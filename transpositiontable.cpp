#include "transpositiontable.h"
#include "constants.h"
#include <iostream>
#include <ctime>
#include <cassert>
namespace Napoleon
{
    TranspositionTable::TranspositionTable(unsigned long size)
    {
        BucketSize = 4;
        Size = size/(sizeof(HashEntry*)*BucketSize);

        //        try
        //        {
        Table = new HashEntry*[Size];
        //        }
        //        catch(std::bad_alloc& ex)
        //        {
        //            std::cout << "Transposition table bad allocation: " << ex.what() << std::endl;
        //        }

        for (unsigned i=0; i<Size; i++)
        {
            Table[i] = new HashEntry[BucketSize];
        }
    }

    void TranspositionTable::Save(ZobristKey key, Byte depth, int score, Move move, ScoreType bound)
    {
        int min = Constants::MaxPly;
        int index = 0;

        HashEntry hash;
        for (int i=0; i<BucketSize; i++)
        {
            hash = Table[key % Size][i];
            if (hash.Depth < min)
            {
                min = hash.Depth;
                index = i;
            }
        }

        if (depth >= min)
        {
            HashEntry& hashToOverride = Table[key % Size][index];

            hashToOverride.Hash = key;
            hashToOverride.Score = score;
            hashToOverride.Depth = depth;
            hashToOverride.Bound = bound;
            hashToOverride.BestMove = move;
        }
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
                    if (hash->Bound == ScoreType::Exact)
                        return hash->Score;
                    if (hash->Bound == ScoreType::Alpha && hash->Score <= alpha)
                        return alpha;
                    if (hash->Bound == ScoreType::Beta && hash->Score >= beta)
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
                if (!hash->BestMove.IsNull())
                    return hash->BestMove;
            }
        }

        return Constants::NullMove;
    }
}

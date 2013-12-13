#include "transpositiontable.h"
#include "constants.h"
#include <iostream>
#include <cmath>
#include <cstring>

namespace Napoleon
{
    TranspositionTable::TranspositionTable(int mb)
    {
        SetSize(mb);
    }

    void TranspositionTable::SetSize(int mb)
    {
        // get a power of two size in megabytes
        mb = std::pow(2, int(std::log2(mb)));

        // mb * 2^x = mb << x   <==> mb = 2^k
        entries = ( (mb*std::pow(2, 20)) / sizeof(HashEntry)); // number of bytes * size of HashEntry = number of entries

        free(table);
        table = (HashEntry*) std::calloc(entries * sizeof(HashEntry), 1);

        mask = entries - BucketSize;
    }

    void TranspositionTable::Save(ZobristKey key, Byte depth, int score, Move move, ScoreType bound)
    {
        int min = Constants::MaxPly;
        int index = 0;
        auto hash = at(key);

        for (auto i=0; i<BucketSize; i++, hash++)
        {
            if (hash->Depth < min)
            {
                min = hash->Depth;
                index = i;
            }
        }

        if (depth >= min)
        {
            auto hashToOverride = at(key, index);

            hashToOverride->Hash = key;
            hashToOverride->Score = score;
            hashToOverride->Depth = depth;
            hashToOverride->Bound = bound;
            hashToOverride->BestMove = move;
        }
    }

    std::pair<int, Move> TranspositionTable::Probe(ZobristKey key, Byte depth, int alpha, int beta)
    {
        auto hash = at(key);
        auto move = Constants::NullMove;

        for (auto i=0; i<BucketSize; i++, hash++)
        {
            if (hash->Hash == key)
            {
                if (hash->Depth >= depth)
                {
                    if (hash->Bound == ScoreType::Exact)
                        return std::make_pair(hash->Score, move);

                    if (hash->Bound == ScoreType::Alpha && hash->Score <= alpha)
                        return std::make_pair(alpha, move);

                    if (hash->Bound == ScoreType::Beta && hash->Score >= beta)
                        return std::make_pair(beta, move);
                }
                move = hash->BestMove; // get best move on this position
            }
        }

        return std::make_pair(Unknown, move);
    }

    void TranspositionTable::Clear()
    {
        std::memset(table, 0, entries*sizeof(HashEntry));
    }

    Move TranspositionTable::GetPv(ZobristKey key)
    {
        auto hash = at(key);

        for (auto i=0; i<BucketSize; i++, hash++)
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

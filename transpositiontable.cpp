#include "transpositiontable.h"
#include "constants.h"
#include "utils.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <cassert>

namespace Napoleon
{    
    const int TranspositionTable::BucketSize = 1;
    const int TranspositionTable::Unknown = -999999;
    unsigned long stores = 0;

    TranspositionTable::TranspositionTable(int mb)
    {
        SetSize(mb);
    }

    void TranspositionTable::SetSize(int mb)
    {
        // get a power of two size in megabytes
        mb = std::pow(2, int(Utils::Math::Log2(mb)));

        // mb * 2^x = mb << x   <==>   mb = 2^k
        entries = ( (mb*std::pow(2, 20)) / sizeof(HashEntry)); // number of bytes * size of HashEntry = number of entries
        lock_entries = (entries / BucketSize);

        free(table);
        free(locks);
        table = (HashEntry*) std::calloc(entries * sizeof(HashEntry), 1);
        locks = new SpinLock[lock_entries];

        mask = entries - BucketSize;
    }

    void TranspositionTable::Save(ZobristKey key, Byte depth, Byte age, int score, Move move, ScoreType bound)
    {
        SpinLock* mux = locks + (key & mask)/BucketSize;;

        if (Concurrent)
            mux->lock();

        //stores++;

        int min = Constants::MaxPly;
        auto hash = at(key);
        HashEntry* hashToOverride = nullptr;

        for (auto i=0; i<BucketSize; i++, hash++)
        {
            /*
            if (age != (hash->Bound >> 2))
            {
                hashToOverride = hash;
                break;
            }
            */
            if (hash->Depth < min)
            {
                min = hash->Depth;
                hashToOverride = hash;
            }
        }

        assert(hashToOverride != nullptr);

        hashToOverride->Hash = key;
        hashToOverride->Score = score;
        hashToOverride->Depth = depth;
        hashToOverride->Bound = (bound | (age << 2));
        hashToOverride->BestMove = move;

        mux->unlock();
    }

    std::pair<int, Move> TranspositionTable::Probe(ZobristKey key, Byte depth, Byte age, int alpha, int beta)
    {
        SpinLock* mux = locks + (key & mask)/BucketSize;

        if (Concurrent)
            mux->lock();

        auto hash = at(key);
        auto move = Constants::NullMove;

        for (auto i=0; i<BucketSize; i++, hash++)
        {
            if (hash->Hash == key)
            {
                hash->Bound = ((hash->Bound & 0x3) | (age << 2));
                if (hash->Depth >= depth)
                {
                    if ((hash->Bound & 0x3) == ScoreType::Exact)
                    { mux->unlock(); return std::make_pair(hash->Score, move); }

                    if ((hash->Bound & 0x3) == ScoreType::Alpha && hash->Score <= alpha)
                    { mux->unlock(); return std::make_pair(alpha, move); }

                    if ((hash->Bound & 0x3) == ScoreType::Beta && hash->Score >= beta)
                    { mux->unlock(); return std::make_pair(beta, move); }
                }
                move = hash->BestMove; // get best move on this position
            }
        }

        mux->unlock();
        return std::make_pair(Unknown, move);
    }

    void TranspositionTable::Clear()
    {
        //std::memset(table, 0, entries*sizeof(HashEntry)/2); // clear first half
        // DO NOT CLEAR

        /*
           auto hash = table;
           unsigned long gen[64] = {0};
           unsigned long empty = 0;
           for (unsigned long i=0; i<entries; i++, hash++)
           {
           if (!hash->Hash)
           {
           empty++;
           }
           else
           {
           gen[(hash->Bound >> 2)]++;
           }
           }
           std::cout << "empty cells: " << 100*empty/entries << "%" << std::endl;
           std::cout << "stores/entries: " << 100*stores/entries << "%" << std::endl;
           std::cout << "generation count: " << std::endl;
           for (auto i=0; i<64; i++)
           {
           if (gen[i] > 0)
           std::cout << "g[" << i << "]=" << 100*gen[i]/entries << "%" << '\t';
           }
           std::cout << std::endl;
           */

    }

    //TODO return BEST pv move (exact score)
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

#include "pawntable.h"
#include "utils.h"

namespace Napoleon
{

    PawnTable::PawnTable()
    {
        entries = ((64*std::pow(2, 20)) / sizeof(PawnEntry)); // number of bytes * size of HashEntry = number of entries
        entries = std::pow(2, Utils::Math::Log2(entries));
        lock_entries = entries;
        free(table);
        free(locks);
        table = (PawnEntry*) std::calloc(entries * sizeof(PawnEntry), 1);
        locks = new SpinLock[lock_entries];
        mask = entries - 1;
    }

    PawnEntry* PawnTable::at(ZobristKey key) const
    {
        return table + (key & mask);
    }

    void PawnTable::Save(ZobristKey key, Score score)
    {
        SpinLock* mux = locks + (key & mask);

        if (Concurrent)
            mux->lock();

        auto hash = at(key);
        hash->key = key;
        hash->score = score;

        mux->unlock();
    }

    Score PawnTable::Probe(ZobristKey key)
    {
        SpinLock* mux = locks + (key & mask);

        if (Concurrent)
            mux->lock();

        auto hash = at(key);
        if (hash->key == key)
        {
            return hash->score;
        }

        mux->unlock();
        return Score(TranspositionTable::Unknown, TranspositionTable::Unknown);
    }

}

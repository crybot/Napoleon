#include "pawntable.h"
#include "utils.h"

namespace Napoleon
{

    PawnTable::PawnTable()
    {
        entries = ((64*std::pow(2, 20)) / sizeof(PawnEntry)); // number of bytes * size of HashEntry = number of entries
        entries = std::pow(2, Utils::Math::Log2(entries));
        free(table);
        table = (PawnEntry*) std::calloc(entries * sizeof(PawnEntry), 1);
        mask = entries - 1;
    }

    PawnEntry* PawnTable::at(ZobristKey key) const
    {
        return table + (key & mask);
    }

    void PawnTable::Save(ZobristKey key, Score score)
    {
        auto hash = at(key);
        hash->key = key;
        hash->score = score;
    }

    Score PawnTable::Probe(ZobristKey key)
    {
        auto hash = at(key);
        if (hash->key == key)
        {
            return hash->score;
        }

        return Score(TranspositionTable::Unknown, TranspositionTable::Unknown);
    }

}

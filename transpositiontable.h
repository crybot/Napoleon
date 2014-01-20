#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H
#include "defines.h"
#include "hashentry.h"

namespace Napoleon
{
    class TranspositionTable
    {
    public:
        static const int Unknown = -999999;
        static const int BucketSize = 4;

        TranspositionTable(int size = 1);

        void SetSize(int);
        void Save(ZobristKey, Byte, int, Move, ScoreType);
        void Clear();
        std::pair<int, Move> Probe(ZobristKey, Byte, int, int);
        Move GetPv(ZobristKey);

    private:
        unsigned long long mask;
        unsigned long entries;
        HashEntry* table;

        HashEntry* at(ZobristKey, int = 0) const;
    };

    inline HashEntry* TranspositionTable::at(ZobristKey key, int index) const
    {
        return table + (key & mask) + index;
    }
}

#endif // TRANSPOSITIONTABLE_H

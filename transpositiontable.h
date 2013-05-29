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

        int BucketSize = 4;
        unsigned long Size;
        HashEntry** Table;

        TranspositionTable(unsigned long size = 1024);

        const HashEntry& operator[](int) const;

        void Save(ZobristKey, Byte, int, Move, Byte);
        int Probe(ZobristKey, Byte, int, Move*, int);
        void Clear();
        Move GetPv(ZobristKey);
    };

    INLINE const HashEntry& TranspositionTable::operator[](int i) const
    {
        return Table[i][0];
    }

}

#endif // TRANSPOSITIONTABLE_H
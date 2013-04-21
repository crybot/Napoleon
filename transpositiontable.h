#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H
#include "defines.h"
#include "hashentry.h"

namespace Napoleon
{
    class TranspositionTable
    {
    public:
        static const int Unknown = -32767;

        unsigned long Size;
        HashEntry* Table;

        TranspositionTable(unsigned long size = 1024);

        void Save(HashEntry);
        int Probe(ZobristKey, int, int, int);
    };

}

#endif // TRANSPOSITIONTABLE_H

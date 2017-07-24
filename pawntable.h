#include "transpositiontable.h"
#include <cmath>

namespace Napoleon
{
    class PawnEntry
    {
        public:
            ZobristKey key;
            Score score;
            BitBoard attacks[2]; // color
            BitBoard passers[2]; // holds (candidate) passers positions for both colors
    };

    class PawnTable 
    {
        public:
            PawnTable();
            void Save(ZobristKey, Score);
            Score Probe(ZobristKey);
            PawnEntry* at(ZobristKey) const;
        private:
            PawnEntry* table;
            unsigned long long mask;
            unsigned long long entries;
    };

}

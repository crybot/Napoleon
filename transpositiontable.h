#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H
#include "defines.h"
#include "hashentry.h"
#include <mutex>
#include <atomic>

namespace Napoleon
{
    //COPIED
    class SpinLock
    {
        public:
            void lock()
            {
                while(lck.test_and_set(std::memory_order_acquire))
                {}
            }

            void unlock()
            {
                lck.clear(std::memory_order_release);
            }

        private:
            std::atomic_flag lck = ATOMIC_FLAG_INIT;
    };

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
            unsigned long lock_entries;
            HashEntry* table;
            SpinLock* locks;
            HashEntry* at(ZobristKey, int = 0) const;
    };

    inline HashEntry* TranspositionTable::at(ZobristKey key, int index) const
    {
        return table + (key & mask) + index;
    }
}

#endif // TRANSPOSITIONTABLE_H

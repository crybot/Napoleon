#ifndef HASHENTRY_H
#define HASHENTRY_H
#include "defines.h"
#include "move.h"
#include <mutex>
#include <atomic>

namespace Napoleon
{
    enum ScoreType : Byte { Exact=0, Alpha=1, Beta=2 };

    class HashEntry
    {
    public:
        ZobristKey Hash;
        Byte Depth;
        Byte Bound; // also holds age
        Move BestMove;
        int Score;

        HashEntry();
        HashEntry(ZobristKey, Byte, Byte, int, Move, ScoreType);
    };
}

#endif // HASHENTRY_H

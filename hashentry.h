#ifndef HASHENTRY_H
#define HASHENTRY_H
#include "defines.h"
#include "move.h"

namespace Napoleon
{
    enum class ScoreType : Byte { Exact, Alpha, Beta };

    class HashEntry
    {
    public:
        ZobristKey Hash;
        Byte Depth;
        ScoreType Bound;
        Move BestMove;
        int Score;

        HashEntry();
        HashEntry(ZobristKey, Byte, int, Move, ScoreType);
    };
}

#endif // HASHENTRY_H

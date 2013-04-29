#ifndef HASHENTRY_H
#define HASHENTRY_H
#include "defines.h"
#include "move.h"

namespace Napoleon
{
    enum ScoreType { Exact, Alpha, Beta };

    class HashEntry
    {
    public:
        ZobristKey Hash;
        Byte Depth;
        Byte Bound;
        Move BestMove;
        int Score;

        HashEntry();
        HashEntry(ZobristKey, Byte, int, Move, Byte);
    };
}

#endif // HASHENTRY_H

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
        int Depth;
        int Score;
        int BestMove;
        Byte Bound;


        HashEntry();
        HashEntry(ZobristKey, int, int, int, Byte);
    };
}

#endif // HASHENTRY_H

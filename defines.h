#ifndef DEFINES_H
#define DEFINES_H
#include <string>

#define C64(constantU64) constantU64##ULL

#ifdef __GNUC__
#   define INLINE __inline __attribute__ ((__always_inline__))
#elif defined(_MSC_VER) && defined(_WIN64)
#   include<intrin.h>
#	pragma intrinsic(_BitScanForward64)
#   define INLINE __forceinline
#else
#   define INLINE inline
#endif

namespace Napoleon
{
    typedef unsigned long long BitBoard;
    typedef unsigned char Type;
    typedef unsigned char Color;
    typedef unsigned char Byte;
    typedef unsigned long long ZobristKey;
    typedef unsigned long long Random64;
    typedef unsigned int Square; // change with unsigned char
    typedef unsigned int File; // change with unsigned char
    typedef unsigned int Rank; // change with unsigned char
    typedef std::pair<int,int> Score;

    inline Score operator- (const Score& score)
    {
        return Score(-score.first, -score.second);
    }
    inline Score operator/ (const Score& score, int d)
    {
        return Score(score.first/d, score.second/d);
    }

    enum GameStage { Opening = 0, MiddleGame = 1, EndGame = 2 };
    enum Operation { Add, Sub };
}

#endif // DEFINES_H

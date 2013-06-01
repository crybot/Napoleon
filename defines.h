#ifndef DEFINES_H
#define DEFINES_H
#include <string>

#define C64(constantU64) constantU64##ULL

#ifdef __GNUC__
#   define INLINE __inline __attribute__ ((__always_inline__))
#elif defined(_MSC_VER)
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
    typedef int Square;
    typedef int MoveType;
}

#endif // DEFINES_H

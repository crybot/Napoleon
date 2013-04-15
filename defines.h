#ifndef DEFINES_H
#define DEFINES_H
#include <string>

#define C64(constantU64) constantU64##ULL

#if __GNUC_PREREQ (3,2)
#   define INLINE __inline __attribute__ ((__always_inline__))
#else
#   define INLINE inline
#endif

namespace Napoleon
{
    typedef unsigned long long BitBoard;
    typedef unsigned char Type;
    typedef unsigned char Color;
    typedef unsigned char Byte;
    typedef int Square;
    typedef unsigned long long ZobristKey;
}

#endif // DEFINES_H

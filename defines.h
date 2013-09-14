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
    using BitBoard = unsigned long long;
    using Type = unsigned char;
    using Color = unsigned char;
    using Byte = unsigned char;
    using ZobristKey = unsigned long long;
    using Square = unsigned int;
    using File = unsigned int;
    using Rank = unsigned int;
    using MoveType = int;
}

#endif // DEFINES_H

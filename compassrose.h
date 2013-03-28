#ifndef COMPASSROSE_H
#define COMPASSROSE_H
#include "constants.h"

namespace Napoleon
{
    namespace CompassRose
    {
        __always_inline constexpr BitBoard OneStepSouth(BitBoard bitBoard) { return bitBoard >> 8; }
        __always_inline constexpr BitBoard OneStepNorth(BitBoard bitBoard) { return bitBoard << 8; }
        __always_inline constexpr BitBoard OneStepWest(BitBoard bitBoard) { return bitBoard >> 1 & Constants::NotHFile; }
        __always_inline constexpr BitBoard OneStepEast(BitBoard bitBoard) { return bitBoard << 1 & Constants::NotAFile; }

        __always_inline constexpr BitBoard OneStepNorthEast(BitBoard bitBoard) { return bitBoard << 9 & Constants::NotAFile; }
        __always_inline constexpr BitBoard OneStepNorthWest(BitBoard bitBoard) { return bitBoard << 7 & Constants::NotHFile; }
        __always_inline constexpr BitBoard OneStepSouthEast(BitBoard bitBoard) { return bitBoard >> 7 & Constants::NotAFile; }
        __always_inline constexpr BitBoard OneStepSouthWest(BitBoard bitBoard) { return bitBoard >> 9 & Constants::NotHFile; }

        //        BitBoard OneStepSouth(BitBoard bitBoard);
        //        BitBoard OneStepNorth(BitBoard bitBoard);
        //        BitBoard OneStepWest(BitBoard bitBoard);
        //        BitBoard OneStepEast(BitBoard bitBoard);

        //        BitBoard OneStepNorthEast(BitBoard bitBoard);
        //        BitBoard OneStepNorthWest(BitBoard bitBoard);
        //        BitBoard OneStepSouthEast(BitBoard bitBoard);
        //        BitBoard OneStepSouthWest(BitBoard bitBoard);
    }
}

#endif // COMPASSROSE_H

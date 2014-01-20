#ifndef COMPASSROSE_H
#define COMPASSROSE_H
#include "constants.h"

namespace Napoleon
{
    namespace CompassRose
    {
        /*      northwest    north   northeast
                noWe         nort         noEa
                        +7    +8    +9
                            \  |  /
                west    -1 <-  0 -> +1    east
                            /  |  \
                        -9    -8    -7
                soWe         sout         soEa
                southwest    south   southeast
        */

        INLINE static BitBoard OneStepSouth(BitBoard bitBoard) { return bitBoard >> 8; }
        INLINE static BitBoard OneStepNorth(BitBoard bitBoard) { return bitBoard << 8; }
        INLINE static BitBoard OneStepWest(BitBoard bitBoard) { return bitBoard >> 1 & Constants::NotHFile; }
        INLINE static BitBoard OneStepEast(BitBoard bitBoard) { return bitBoard << 1 & Constants::NotAFile; }

        INLINE static BitBoard OneStepNorthEast(BitBoard bitBoard) { return bitBoard << 9 & Constants::NotAFile; }
        INLINE static BitBoard OneStepNorthWest(BitBoard bitBoard) { return bitBoard << 7 & Constants::NotHFile; }
        INLINE static BitBoard OneStepSouthEast(BitBoard bitBoard) { return bitBoard >> 7 & Constants::NotAFile; }
        INLINE static BitBoard OneStepSouthWest(BitBoard bitBoard) { return bitBoard >> 9 & Constants::NotHFile; }
    }
}

#endif // COMPASSROSE_H

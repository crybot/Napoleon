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

        INLINE const BitBoard OneStepSouth(BitBoard bitBoard) { return bitBoard >> 8; }
        INLINE const BitBoard OneStepNorth(BitBoard bitBoard) { return bitBoard << 8; }
        INLINE const BitBoard OneStepWest(BitBoard bitBoard) { return bitBoard >> 1 & Constants::NotHFile; }
        INLINE const BitBoard OneStepEast(BitBoard bitBoard) { return bitBoard << 1 & Constants::NotAFile; }

        INLINE const BitBoard OneStepNorthEast(BitBoard bitBoard) { return bitBoard << 9 & Constants::NotAFile; }
        INLINE const BitBoard OneStepNorthWest(BitBoard bitBoard) { return bitBoard << 7 & Constants::NotHFile; }
        INLINE const BitBoard OneStepSouthEast(BitBoard bitBoard) { return bitBoard >> 7 & Constants::NotAFile; }
        INLINE const BitBoard OneStepSouthWest(BitBoard bitBoard) { return bitBoard >> 9 & Constants::NotHFile; }
    }
}

#endif // COMPASSROSE_H

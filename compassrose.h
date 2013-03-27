#ifndef COMPASSROSE_H
#define COMPASSROSE_H
#include "defines.h"

namespace Napoleon
{
    namespace CompassRose
    {
        BitBoard OneStepSouth(BitBoard bitBoard);
        BitBoard OneStepNorth(BitBoard bitBoard);
        BitBoard OneStepWest(BitBoard bitBoard);
        BitBoard OneStepEast(BitBoard bitBoard);

        BitBoard OneStepNorthEast(BitBoard bitBoard);
        BitBoard OneStepNorthWest(BitBoard bitBoard);
        BitBoard OneStepSouthEast(BitBoard bitBoard);
        BitBoard OneStepSouthWest(BitBoard bitBoard);
    }
}

#endif // COMPASSROSE_H

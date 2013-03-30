//#include "compassrose.h"
//#include "constants.h"

//namespace Napoleon
//{
//    namespace CompassRose
//    {

//        /* northwest    north   northeast
//        noWe         nort         noEa
//                +7    +8    +9
//                    \  |  /
//        west    -1 <-  0 -> +1    east
//                    /  |  \
//                -9    -8    -7
//        soWe         sout         soEa
//        southwest    south   southeast
//        */

//        BitBoard OneStepSouth(BitBoard bitBoard) { return bitBoard >> 8; }
//        BitBoard OneStepNorth(BitBoard bitBoard) { return bitBoard << 8; }
//        BitBoard OneStepWest(BitBoard bitBoard) { return bitBoard >> 1 & Constants::NotHFile; }
//        BitBoard OneStepEast(BitBoard bitBoard) { return bitBoard << 1 & Constants::NotAFile; }

//        BitBoard OneStepNorthEast(BitBoard bitBoard) { return bitBoard << 9 & Constants::NotAFile; }
//        BitBoard OneStepNorthWest(BitBoard bitBoard) { return bitBoard << 7 & Constants::NotHFile; }
//        BitBoard OneStepSouthEast(BitBoard bitBoard) { return bitBoard >> 7 & Constants::NotAFile; }
//        BitBoard OneStepSouthWest(BitBoard bitBoard) { return bitBoard >> 9 & Constants::NotHFile; }
//    }
//}

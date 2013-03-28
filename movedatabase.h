#ifndef MOVEDATABASE_H
#define MOVEDATABASE_H
#include "defines.h"

namespace Napoleon
{
    class MoveDatabase
    {
    public:
        static BitBoard WhitePawnAttacks[64]; // square
        static BitBoard BlackPawnAttacks[64];// square
        static BitBoard KingAttacks[64]; // square
        static BitBoard KnightAttacks[64]; // square

        static BitBoard PseudoRookAttacks[64]; // square
        static BitBoard PseudoBishopAttacks[64]; // square

        static BitBoard GetRankAttacks(BitBoard, int);
        static BitBoard GetFileAttacks(BitBoard, int);
        static BitBoard GetA1H8DiagonalAttacks(BitBoard, int);
        static BitBoard GetH1A8DiagonalAttacks(BitBoard, int);
        static void InitAttacks();

    private:
        static BitBoard RankAttacks[64][64]; // square , occupancy
        static BitBoard FileAttacks[64][64]; // square , occupancy
        static BitBoard A1H8DiagonalAttacks[64][64]; // square , occupancy
        static BitBoard H1A8DiagonalAttacks[64][64]; // square , occupancy

        static void initPawnAttacks();
        static void initKnightAttacks();
        static void initKingAttacks();
        static void initRankAttacks();
        static void initFileAttacks();
        static void initDiagonalAttacks();
        static void initAntiDiagonalAttacks();
        static void initPseudoAttacks();


    };
}

#endif // MOVEDATABASE_H

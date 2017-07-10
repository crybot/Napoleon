#ifndef MOVEDATABASE_H
#define MOVEDATABASE_H
#include <immintrin.h>
#include "defines.h"
#include "utils.h"
#include "constants.h"
#include <iostream>

namespace Napoleon
{
    class MoveDatabase
    {
    public:
        static BitBoard PawnAttacks[2][64]; // color, square
        static BitBoard KingAttacks[64]; // square
        static BitBoard KnightAttacks[64]; // square

        static BitBoard PseudoRookAttacks[64]; // square
        static BitBoard PseudoBishopAttacks[64]; // square
        static BitBoard ObstructedTable[64][64]; // square, square
        static BitBoard KingProximity[2][64]; // color, square
        static BitBoard SideFiles[8]; // file
        static BitBoard FrontSpan[2][64]; // color, square
        static BitBoard PasserSpan[2][64]; // color, square
        static int Distance[64][64]; // square, square

        static BitBoard GetA1H8DiagonalAttacks(BitBoard, Square);
        static BitBoard GetH1A8DiagonalAttacks(BitBoard, Square);
        static bool AreSquareAligned(Square, Square, Square);
        static void InitAttacks();

        static BitBoard RookAttacks[64][64*64]; // square, occupancy (12 bits)

        static BitBoard GetRookAttacks(BitBoard, Square);

    private:
        static BitBoard RankAttacks[64][64]; // square , occupancy
        static BitBoard FileAttacks[64][64]; // square , occupancy
        static BitBoard A1H8DiagonalAttacks[64][64]; // square , occupancy
        static BitBoard H1A8DiagonalAttacks[64][64]; // square , occupancy

        static BitBoard RookMask[64];

        static void initPawnAttacks();
        static void initKnightAttacks();
        static void initKingAttacks();
        static void initRankAttacks();
        static void initFileAttacks();
        static void initDiagonalAttacks();
        static void initAntiDiagonalAttacks();
        static void initPseudoAttacks();
        static void initObstructedTable();

        static void initRookAttacks();
    };

    INLINE BitBoard MoveDatabase::GetRookAttacks(BitBoard occupiedSquares, Square square)
    {
        auto pext = _pext_u64(occupiedSquares, MoveDatabase::RookMask[square]);
        return MoveDatabase::RookAttacks[square][pext];
    }

    INLINE BitBoard MoveDatabase::GetA1H8DiagonalAttacks(BitBoard occupiedSquares, Square square)
    {
        int diag = Utils::Square::GetA1H8DiagonalIndex(square);
        //int occupancy = (int)((occupiedSquares & Constants::Masks::A1H8DiagonalMask[diag]) * Constants::Magics::A1H8DiagonalMagic[diag] >> 56);
        auto pext = _pext_u64(occupiedSquares, Constants::Masks::SixBitA1H8DiagonalMask[diag]);
        return A1H8DiagonalAttacks[square][pext];
    }

    INLINE BitBoard MoveDatabase::GetH1A8DiagonalAttacks(BitBoard occupiedSquares, Square square)
    {
        int diag = Utils::Square::GetH1A8AntiDiagonalIndex(square);
        //int occupancy = (int)((occupiedSquares & Constants::Masks::H1A8DiagonalMask[diag]) * Constants::Magics::H1A8DiagonalMagic[diag] >> 56);
        auto pext = _pext_u64(occupiedSquares, Constants::Masks::SixBitH1A8DiagonalMask[diag]);
        return H1A8DiagonalAttacks[square][pext];
    }

    INLINE bool MoveDatabase::AreSquareAligned(Square s1, Square s2, Square s3)
    {
        return (ObstructedTable[s1][s2] | ObstructedTable[s1][s3] | ObstructedTable[s2][s3])
            & (Constants::Masks::SquareMask[s1] | Constants::Masks::SquareMask[s2] | Constants::Masks::SquareMask[s3]);
    }
}

#endif // MOVEDATABASE_H

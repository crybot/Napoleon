#ifndef MOVEDATABASE_H
#define MOVEDATABASE_H
#include "defines.h"
#include "utils.h"
#include "constants.h"

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
        static BitBoard ObstructedTable[64][64]; // square, square

        static BitBoard GetRankAttacks(BitBoard, int);
        static BitBoard GetFileAttacks(BitBoard, int);
        static BitBoard GetA1H8DiagonalAttacks(BitBoard, int);
        static BitBoard GetH1A8DiagonalAttacks(BitBoard, int);
        static bool AreSquareAligned(int, int, int);
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
        static void initObstructedTable();
    };

    INLINE BitBoard MoveDatabase::GetRankAttacks(BitBoard occupiedSquares, int square)
    {
        int rank = Utils::Square::GetRankIndex(square);
        int occupancy = (int)((occupiedSquares & Constants::Masks::SixBitRankMask[rank]) >> (8 * rank));
        return RankAttacks[square][(occupancy >> 1) & 63];
    }

    INLINE BitBoard MoveDatabase::GetFileAttacks(BitBoard occupiedSquares, int square)
    {
        int file = Utils::Square::GetFileIndex(square);
        int occupancy = (int)((occupiedSquares & Constants::Masks::SixBitFileMask[file]) * Constants::Magics::FileMagic[file] >> 56);
        return FileAttacks[square][(occupancy >> 1) & 63];
    }

    INLINE BitBoard MoveDatabase::GetA1H8DiagonalAttacks(BitBoard occupiedSquares, int square)
    {
        int diag = Utils::Square::GetA1H8DiagonalIndex(square);
        int occupancy = (int)((occupiedSquares & Constants::Masks::A1H8DiagonalMask[diag]) * Constants::Magics::A1H8DiagonalMagic[diag] >> 56);
        return A1H8DiagonalAttacks[square][(occupancy >> 1) & 63];
    }

    INLINE BitBoard MoveDatabase::GetH1A8DiagonalAttacks(BitBoard occupiedSquares, int square)
    {
        int diag = Utils::Square::GetH1A8AntiDiagonalIndex(square);
        int occupancy = (int)((occupiedSquares & Constants::Masks::H1A8DiagonalMask[diag]) * Constants::Magics::H1A8DiagonalMagic[diag] >> 56);
        return H1A8DiagonalAttacks[square][(occupancy >> 1) & 63];
    }

    INLINE bool MoveDatabase::AreSquareAligned(int s1, int s2, int s3)
    {
        return ((ObstructedTable[s1][s2] | ObstructedTable[s1][s3] | ObstructedTable[s2][s3])
                          & (Constants::Masks::SquareMask[s1] | Constants::Masks::SquareMask[s2] | Constants::Masks::SquareMask[s3])) != 0;
    }

}

#endif // MOVEDATABASE_H

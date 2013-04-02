#include "movedatabase.h"
#include "compassrose.h"
#include "knight.h"
#include "king.h"

namespace Napoleon
{
    BitBoard MoveDatabase::WhitePawnAttacks[64]; // square
    BitBoard MoveDatabase::BlackPawnAttacks[64];// square
    BitBoard MoveDatabase::KingAttacks[64]; // square
    BitBoard MoveDatabase::KnightAttacks[64]; // square

    BitBoard MoveDatabase::RankAttacks[64][64]; // square, occupancy
    BitBoard MoveDatabase::FileAttacks[64][64]; // square, occupancy
    BitBoard MoveDatabase::A1H8DiagonalAttacks[64][64]; // square , occupancy
    BitBoard MoveDatabase::H1A8DiagonalAttacks[64][64]; // square , occupancy
    BitBoard MoveDatabase::PseudoRookAttacks[64]; // square
    BitBoard MoveDatabase::PseudoBishopAttacks[64]; // square

    BitBoard MoveDatabase::ObstructedTable[64][64];


    /* all this operation will be executed before the engine gets active, so it is not needed optimization */

    void MoveDatabase::InitAttacks()
    {
        initPawnAttacks();
        initKnightAttacks();
        initKingAttacks();
        initRankAttacks();
        initFileAttacks();
        initDiagonalAttacks();
        initAntiDiagonalAttacks();
        initPseudoAttacks();
        initObstructedTable();
    }

    void MoveDatabase::initPawnAttacks()
    {
        for (int sq = 0; sq < 64; sq++)
        {
            WhitePawnAttacks[sq] = CompassRose::OneStepNorthEast(Constants::Masks::SquareMask[sq]) | CompassRose::OneStepNorthWest(Constants::Masks::SquareMask[sq]);
            BlackPawnAttacks[sq] = CompassRose::OneStepSouthEast(Constants::Masks::SquareMask[sq]) | CompassRose::OneStepSouthWest(Constants::Masks::SquareMask[sq]);
        }
    }

    void MoveDatabase::initKnightAttacks()
    {
        // inizializza l'array di mosse precalcolate
        for (int sq = 0; sq < 64; sq++)
        {
            KnightAttacks[sq] = Knight::GetKnightAttacks(Constants::Masks::SquareMask[sq]);
        }
    }
    void MoveDatabase::initKingAttacks()
    {
        for (int sq = 0; sq < 64; sq++)
        {
            // inizializza l'array di mosse precalcolate
            KingAttacks[sq] = King::GetKingAttacks(Constants::Masks::SquareMask[sq]);
        }
    }

    void MoveDatabase::initRankAttacks()
    {
        for (int sq = 0; sq < 64; sq++)
        {
            for (int occ = 0; occ < 64; occ++)
            {
                int rank = Utils::Square::GetRankIndex(sq);
                int file = Utils::Square::GetFileIndex(sq);

                BitBoard occupancy = (BitBoard)(occ << 1);
                BitBoard targets = Constants::Empty;

                int blocker = file + 1;
                while (blocker <= 7)
                {
                    targets |= Constants::Masks::SquareMask[blocker];
                    if (Utils::BitBoard::IsBitSet(occupancy, blocker)) break;

                    blocker++;
                }

                blocker = file - 1;
                while (blocker >= 0)
                {
                    targets |= Constants::Masks::SquareMask[blocker];
                    if (Utils::BitBoard::IsBitSet(occupancy, blocker)) break;

                    blocker--;
                }

                RankAttacks[sq][occ] = targets << (8 * rank);
            }
        }
    }
    void MoveDatabase::initFileAttacks()
    {
        for (int sq = 0; sq < 64; sq++)
        {
            for (int occ = 0; occ < 64; occ++)
            {
                BitBoard targets = Constants::Empty;
                BitBoard rankTargets = RankAttacks[7 - (sq / 8)][occ]; // converte la posizione reale in quella scalare RANK

                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
                {
                    int rank = 7 - bit;
                    int file = Utils::Square::GetFileIndex(sq);

                    if (Utils::BitBoard::IsBitSet(rankTargets, bit))
                    {
                        targets |= Constants::Masks::SquareMask[Utils::Square::GetSquareIndex(file, rank)];
                    }
                }
                FileAttacks[sq][occ] = targets;
            }
        }
    }

    void MoveDatabase::initDiagonalAttacks()
    {

        for (int sq = 0; sq < 64; sq++)
        {
            for (int occ = 0; occ < 64; occ++)
            {
                int diag = Utils::Square::GetRankIndex(sq) - Utils::Square::GetFileIndex(sq);
                BitBoard targets = Constants::Empty;
                BitBoard rankTargets = diag > 0 ? RankAttacks[sq % 8][occ] : RankAttacks[sq / 8][occ];
                // converte la posizione reale in quella scalare RANK //

                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
                {
                    int rank;
                    int file;

                    if (Utils::BitBoard::IsBitSet(rankTargets, bit))
                    {
                        if (diag >= 0)
                        {
                            rank = diag + bit;
                            file = bit;
                        }
                        else
                        {
                            file = bit - diag;
                            rank = bit;
                        }
                        if ((file >= 0) && (file <= 7) && (rank >= 0) && (rank <= 7))
                        {
                            targets |= Constants::Masks::SquareMask[Utils::Square::GetSquareIndex(file, rank)];
                        }
                    }
                }

                A1H8DiagonalAttacks[sq][occ] = targets;
            }
        }
    }

    void MoveDatabase::initAntiDiagonalAttacks()
    {
        for (int sq = 0; sq < 64; sq++)
        {
            for (int occ = 0; occ < 64; occ++)
            {
                int diag = Utils::Square::GetH1A8AntiDiagonalIndex(sq);

                BitBoard targets = Constants::Empty;
                BitBoard rankTargets = diag > 7 ? RankAttacks[7 - sq / 8][occ] : RankAttacks[sq % 8][occ];
                // converte la posizione reale in quella scalare RANK //

                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
                {
                    int rank;
                    int file;

                    if (Utils::BitBoard::IsBitSet(rankTargets, bit))
                    {
                        if (diag >= 7)
                        {
                            rank = 7 - bit;
                            file = (diag - 7) + bit;
                        }
                        else
                        {
                            rank = diag - bit;
                            file = bit;
                        }
                        if ((file >= 0) && (file <= 7) && (rank >= 0) && (rank <= 7))
                        {
                            targets |= Constants::Masks::SquareMask[Utils::Square::GetSquareIndex(file, rank)];
                        }
                    }
                }

                H1A8DiagonalAttacks[sq][occ] = targets;
            }
        }
    }

    void MoveDatabase::initPseudoAttacks()
    {
        for (int i = 0; i < 64; i++)
        {
            PseudoRookAttacks[i] = RankAttacks[i][0] | FileAttacks[i][0];
            PseudoBishopAttacks[i] = A1H8DiagonalAttacks[i][0] | H1A8DiagonalAttacks[i][0];
        }
    }

    void MoveDatabase::initObstructedTable()
    {
        const BitBoard m1   = C64(-1);
        const BitBoard a2a7 = C64(0x0001010101010100);
        const BitBoard b2g7 = C64(0x0040201008040200);
        const BitBoard h1b7 = C64(0x0002040810204080);
        BitBoard btwn, line, rank, file;

        for (int sq1 = 0; sq1<64; sq1++)
        {
            for (int sq2 = 0; sq2<64; sq2++)
            {
                btwn  = (m1 << sq1) ^ (m1 << sq2);
                file  =   (sq2 & 7) - (sq1   & 7);
                rank  =  ((sq2 | 7) -  sq1) >> 3 ;
                line  =      (   (file  & 0xff) - 1) & a2a7;
                line += 2 * ((   (rank  & 0xff) - 1) >> 58);
                line += (((rank - file) & 0xff) - 1) & b2g7;
                line += (((rank + file) & 0xff) - 1) & h1b7; // h1b7 if same antidiag
                line *= btwn & -btwn; // mul acts like shift by smaller square
                ObstructedTable[sq1][sq2] =  line & btwn;   // return the bits on that line inbetween
            }
        }
    }

}

#include "movedatabase.h"
#include "compassrose.h"
#include "knight.h"
#include "king.h"
#include <algorithm>

namespace Napoleon
{
    BitBoard MoveDatabase::PawnAttacks[2][64]; // color, square
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
            PawnAttacks[PieceColor::White][sq] = CompassRose::OneStepNorthEast(Constants::Masks::SquareMask[sq]) | CompassRose::OneStepNorthWest(Constants::Masks::SquareMask[sq]);
            PawnAttacks[PieceColor::Black][sq] = CompassRose::OneStepSouthEast(Constants::Masks::SquareMask[sq]) | CompassRose::OneStepSouthWest(Constants::Masks::SquareMask[sq]);
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
                BitBoard rankTargets = RankAttacks[(sq / 8)][occ]; // converte la posizione reale in quella scalare RANK

                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
                {
                    int rank = bit;
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
                BitBoard rankTargets = diag > 7 ? RankAttacks[7 - (sq % 8)][occ] : RankAttacks[sq / 8][occ];
                // converte la posizione reale in quella scalare RANK //

                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
                {
                    int rank;
                    int file;

                    if (Utils::BitBoard::IsBitSet(rankTargets, bit))
                    {
                        if (diag >= 7)
                        {
                            rank = (diag-7) + bit;
                            file = 7 - bit;
                        }
                        else
                        {
                            rank = bit;
                            file = diag - bit;
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

    // thanks stockfish for this
    void MoveDatabase::initObstructedTable()
    {
        using namespace Utils::Square;

        for (int s1 = 0; s1 < 64; s1++)
        {
            for (int s2 = 0; s2 < 64; s2++)
            {
                ObstructedTable[s1][s2] = 0;
                if ((PseudoRookAttacks[s1] | PseudoBishopAttacks[s1]) & Constants::Masks::SquareMask[s2])
                {
                    int delta = (s2 - s1) / std::max(  abs(GetFileIndex(s1) - GetFileIndex(s2)), abs(GetRankIndex(s1) - GetRankIndex(s2))  );

                    for (int s = s1 + delta; s != s2; s += delta)
                        ObstructedTable[s1][s2] |= Constants::Masks::SquareMask[s];
                }
            }
        }
    }

}

#include "movedatabase.h"
#include "constants.h"
#include "compassrose.h"
#include "utils.h"
#include "knight.h"
#include "king.h"

namespace Napoleon
{
    BitBoard MoveDatabase::WhitePawnAttacks[64]; // square
    BitBoard MoveDatabase::BlackPawnAttacks[64];// square
    BitBoard MoveDatabase::KingAttacks[64]; // square
    BitBoard MoveDatabase::KnightAttacks[64]; // square

    void MoveDatabase::InitAttacks()
    {
        initPawnAttacks();
        initKnightAttacks();
        initKingAttacks();
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
            //            Utils::BitBoard::Display(KnightAttacks[sq] | Constants::Masks::SquareMask[sq]);
            //            std::cin.get();
        }
    }
    void MoveDatabase::initKingAttacks()
    {
        for (int sq = 0; sq < 64; sq++)
        {
            // inizializza l'array di mosse precalcolate
            KingAttacks[sq] = King::GetKingAttacks(Constants::Masks::SquareMask[sq]);
            //            Utils::BitBoard::Display(KingAttacks[sq]);
            //            std::cin.get();
        }
    }
    //    static void InitRankAttacks()
    //    {
    //        for (int sq = 0; sq < 64; sq++)
    //        {
    //            for (int occ = 0; occ < 64; occ++)
    //            {
    //                int rank = Square::GetRankIndex(sq);
    //                int file = Square.GetFileIndex(sq);

    //                BitBoard occupancy = BitBoard.ToBitBoard(occ << 1);
    //                BitBoard targets = Constants.Empty;

    //                int blocker = file + 1;
    //                while (blocker <= 7)
    //                {
    //                    targets |= Constants.SquareMask[blocker];
    //                    if (BitBoard.IsBitSet(occupancy, blocker)) break;

    //                    blocker++;
    //                }

    //                blocker = file - 1;
    //                while (blocker >= 0)
    //                {
    //                    targets |= Constants.SquareMask[blocker];
    //                    if (BitBoard.IsBitSet(occupancy, blocker)) break;

    //                    blocker--;
    //                }

    //                RankAttacks[sq][occ] = targets << (8 * rank);
    //            }
    //        }
    //    }
    //    static void InitFileAttacks()
    //    {
    //        for (int i = 0; i < 64; i++)
    //        {
    //            FileAttacks[i] = new BitBoard[64];
    //        }

    //        for (int sq = 0; sq < 64; sq++)
    //        {
    //            for (int occ = 0; occ < 64; occ++)
    //            {
    //                BitBoard targets = Constants.Empty;
    //                BitBoard rankTargets = RankAttacks[7 - (sq / 8)][occ]; // converte la posizione reale in quella scalare RANK

    //                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
    //                {
    //                    int rank = 7 - bit;
    //                    int file = Square.GetFileIndex(sq);

    //                    if (BitBoard.IsBitSet(rankTargets, bit))
    //                    {
    //                        targets |= Constants.SquareMask[Square.GetSquareIndex(file, rank)];
    //                    }
    //                }
    //                FileAttacks[sq][occ] = targets;
    //            }
    //        }
    //    }
    //    static void InitDiagonalAttacks()
    //    {
    //        for (int i = 0; i < 64; i++)
    //        {
    //            A1H8DiagonalAttacks[i] = new BitBoard[64];
    //        }

    //        for (int sq = 0; sq < 64; sq++)
    //        {
    //            for (int occ = 0; occ < 64; occ++)
    //            {
    //                int diag = Square.GetRankIndex(sq) - Square.GetFileIndex(sq);
    //                BitBoard targets = Constants.Empty;
    //                BitBoard rankTargets = diag > 0 ? RankAttacks[sq % 8][occ] : RankAttacks[sq / 8][occ];
    //                // converte la posizione reale in quella scalare RANK //

    //                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
    //                {
    //                    int rank;
    //                    int file;

    //                    if (BitBoard.IsBitSet(rankTargets, bit))
    //                    {
    //                        if (diag >= 0)
    //                        {
    //                            rank = diag + bit;
    //                            file = bit;
    //                        }
    //                        else
    //                        {
    //                            file = bit - diag;
    //                            rank = bit;
    //                        }
    //                        if ((file >= 0) && (file <= 7) && (rank >= 0) && (rank <= 7))
    //                        {
    //                            targets |= Constants.SquareMask[Square.GetSquareIndex(file, rank)];
    //                        }
    //                    }
    //                }

    //                A1H8DiagonalAttacks[sq][occ] = targets;
    //            }
    //        }
    //    }
    //    static void InitAntiDiagonalAttacks()
    //    {
    //        for (int i = 0; i < 64; i++)
    //        {
    //            H1A8DiagonalAttacks[i] = new BitBoard[64];
    //        }

    //        for (int sq = 0; sq < 64; sq++)
    //        {
    //            for (int occ = 0; occ < 64; occ++)
    //            {
    //                int diag = Square.GetH1A8AntiDiagonalIndex(sq);

    //                BitBoard targets = Constants.Empty;
    //                BitBoard rankTargets = diag > 7 ? RankAttacks[7 - sq / 8][occ] : RankAttacks[sq % 8][occ];
    //                // converte la posizione reale in quella scalare RANK //

    //                for (int bit = 0; bit < 8; bit++) // accede ai singoli bit della traversa (RANK)
    //                {
    //                    int rank;
    //                    int file;

    //                    if (BitBoard.IsBitSet(rankTargets, bit))
    //                    {
    //                        if (diag >= 7)
    //                        {
    //                            rank = 7 - bit;
    //                            file = (diag - 7) + bit;
    //                        }
    //                        else
    //                        {
    //                            rank = diag - bit;
    //                            file = bit;
    //                        }
    //                        if ((file >= 0) && (file <= 7) && (rank >= 0) && (rank <= 7))
    //                        {
    //                            targets |= Constants.SquareMask[Square.GetSquareIndex(file, rank)];
    //                        }
    //                    }
    //                }

    //                H1A8DiagonalAttacks[sq][occ] = targets;
    //            }
    //        }
    //    }
    //    static void InitPseudoAttacks()
    //    {
    //        for (int i = 0; i < 64; i++)
    //        {
    //            PseudoRookAttacks[i] = RankAttacks[i][0] | FileAttacks[i][0];
    //            PseudoBishopAttacks[i] = A1H8DiagonalAttacks[i][0] | H1A8DiagonalAttacks[i][0];
    //        }
    //    }

}

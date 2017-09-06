#include "evaluation.h"
#include "piece.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"
#include "queen.h"
#include <cassert>

namespace Napoleon
{
    PawnTable Evaluation::pawnTable;

    int Evaluation::multiPawnP[8] = { 0, 0, 10, 20, 35, 50, 75, 100 }; // TODO: add phase dependent penalties
    int Evaluation::isolatedPawnP[8] = { 5, 7, 10, 18, 18, 10, 7, 5 }; // TODO: add phase dependent penalties
    int Evaluation::passedPawn[3][8] = // phase, rank
    {
        {0, 0, 10, 15, 25, 30, 30, 0}, // OPENING
        {0},                        // MIDDLEGAME (not used)
        {0, 0, 10, 20, 40, 60, 125, 0} // ENDGAME
    };

    int Evaluation::candidatePawn[3][8] = // phase, rank
    {
        {0, 0, 5, 5, 8, 10, 0, 0}, // OPENING
        {0, 0, 0, 0, 0, 0, 0, 0}, // MIDDLEGAME (not used)
        {0, 0, 10, 15, 20, 30, 0, 0} // ENDGAME
    };

    int Evaluation::supportedPassed[3] = // phase
    {
        0,
        0,
        5
    };

    int Evaluation::pawnIslandsP[3][5] = 
    {
        {0, 0, 5, 7, 9}, // OPENING
        {0, 0, 0, 0}, // MIDDLEGAME (not used)
        {0, 0, 8, 10, 12} // ENDGAME
    };

    int Evaluation::mobilityBonus[3][7][Constants::QueenMaxMoves + 1] =
    {
        /* OPENING */
        {
            {}, // PAWNS
            {-15, -10, -5, 0, 5, 10, 10, 15, 15}, // KNIGHTS
            {-15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 30, 35, 35, 35}, // BISHOPS
            {-5, -5, 0, 5, 10, 10, 15, 20, 30, 35, 35, 40, 40, 40, 40}, // ROOKS
            {-5, -4, -3, -2, -1, 0, 5, 10,
                13, 16, 18, 20, 22, 24, 26, 28,
                29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30}, // QUEENS
            {}, // KING
            {} // NONE
        },

        /* MIDDLEGAME (not used) */
        {

        },

        /* ENDGAME */
        {
            {}, // PAWNS
            {-10, -5, 0, 1, 3, 7, 7, 10, 10}, // KNIGHTS
            {-25, -20, -10, -5, 0, 7, 14, 22, 30, 35, 35, 38, 39, 40}, // BISHOPS
            {-20, -15, -10, 0, 10, 15, 19, 25, 35, 38, 42, 45, 45, 45, 45}, // ROOKS
            {-15, -10, -7, -5, -2, 0, 5, 7,
                10, 14, 18, 20, 22, 24, 26, 28,
                29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30}, // QUEENS
            {}, // KING
            {} // NONE
        }
    };
    /*
       {}, // PAWNS
       {-10, -8, -5, 0, 5, 8, 9, 10, 10}, // KNIGHTS
       {-10, -8, -5, 0, 5, 10, 12, 15, 18, 23, 25, 27, 27, 27}, // BISHOPS
       {-5, -5, -5, 0, 5, 8, 10, 13, 17, 23, 29, 32, 33, 33, 33}, // ROOKS
       {-5, -4, -3, -2, -1, 0, 3, 6,
       9, 13, 16, 18, 20, 22, 24, 26,
       27, 28, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30}, // QUEENS
       {}, // KING
       {}, // NONE
       */

    int Evaluation::backwardPawnP[3] = // phase
    {
        3, // OPENING
        0, // MIDDLEGAME (not used)
        5 // ENDGAME
    };

    int Evaluation::kingAttacks[200] = { // f(x) = 500/(1.0 + std::exp(-x/5.1 + 7.0) + std::min(i, 20))
        0, 1, 2, 3, 4, 6, 7, 8, 10, 11, 
        13, 14, 16, 18, 20, 23, 26, 29, 33, 37, 
        42, 46, 51, 58, 65, 74, 84, 96, 110, 125, 
        143, 162, 183, 205, 228, 252, 277, 301, 325, 348, 
        369, 389, 407, 423, 437, 450, 461, 470, 478, 485, 
        491, 496, 500, 503, 506, 508, 510, 512, 513, 514, 
        515, 516, 517, 517, 518, 518, 518, 518, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
        519, 519, 519, 519, 519, 519, 519, 519, 519, 519, 
    };

    BitBoard Evaluation::pawnAttacks[2]; // color
    BitBoard Evaluation::unpinnedKnightAttacks[2]; // color
    BitBoard attacks[2] = {0}; // all squares attacked by each color
    int Evaluation::hangingValue[2] = {0}; // color

    int kingAttacksCount[2] = {0}; // color


    int Evaluation::Evaluate(Board& board)
    {
        using namespace Constants::Squares;
        using namespace Constants::Castle;
        using namespace Constants::Masks;
        using namespace Constants::Eval;
        using namespace Utils::BitBoard;
        using namespace Utils::Square;
        using namespace Utils::Piece;
        using namespace CompassRose;
        using namespace PieceColor;

        Score scores(0, 0);
        Score pawnStructure(0,0);
        Score wPstValues, bPstValues;

        auto wking_square = board.KingSquare(White);
        auto bking_square = board.KingSquare(Black);

        File kingFile[2] = { // color
            (File)GetFileIndex(wking_square), (File)GetFileIndex(bking_square)
        };

        BitBoard king_proximity[2] = { // color
            MoveDatabase::KingProximity[White][wking_square],
            MoveDatabase::KingProximity[Black][bking_square]
        };

        attacks[White] = attacks[Black] = 0;
        unpinnedKnightAttacks[White] = unpinnedKnightAttacks[Black] = 0;
        hangingValue[White] = hangingValue[Black] = 0;
        kingAttacksCount[White] = kingAttacksCount[Black] = 0;

        // material evaluation
        int material = board.MaterialBalance(White);

        // Piece Square Value evaluation
        wPstValues = board.PstValue(White);
        bPstValues = board.PstValue(Black);

        updateScore(scores, material + (wPstValues.first - bPstValues.first), material + (wPstValues.second - bPstValues.second));

        /* PHASE-DEPENDENT piece bonus*/
        int wKnights = board.NumOfPieces(White, Knight);
        int bKnights = board.NumOfPieces(Black, Knight);
        updateScore(scores, KnightBonus[Opening]*wKnights, KnightBonus[EndGame]*wKnights);
        updateScore(scores, -KnightBonus[Opening]*bKnights, -KnightBonus[EndGame]*bKnights);

        int wRooks = board.NumOfPieces(White, Rook);
        int bRooks = board.NumOfPieces(Black, Rook);
        updateScore(scores, RookBonus[Opening]*wRooks, RookBonus[EndGame]*wRooks);
        updateScore(scores, -RookBonus[Opening]*bRooks, -RookBonus[EndGame]*bRooks);


        // premature queen development
        if (!board.IsOnSquare(White, PieceType::Queen, IntD1))
            updateScore(scores, -15, 0);

        if (!board.IsOnSquare(Black, PieceType::Queen, IntD8))
            updateScore(scores, 15, 0);

        // tempo bonus
        if (board.SideToMove() == White)
            updateScore(scores, 5);
        else
            updateScore(scores, -5);

        // bishop pair bonus
        if (board.NumOfPieces(White, PieceType::Bishop) == 2)
            updateScore(scores, BishopPair[Opening], BishopPair[EndGame]);

        if (board.NumOfPieces(Black, PieceType::Bishop) == 2)
            updateScore(scores, -BishopPair[Opening], -BishopPair[EndGame]);


        auto entry = pawnTable.at(board.pawnKey); 
        bool probed = false;
        if (entry->key == board.pawnKey) // PAWN HASH-HIT
        {
            pawnStructure = entry->score;
            pawnAttacks[White] = entry->attacks[White]; 
            pawnAttacks[Black] = entry->attacks[Black];
            probed = true;
        }

        /* PAWN STRUCTURE */    
        // doubled/isolated pawns evaluation

        BitBoard wpawns = board.Pieces(White, Pawn);
        BitBoard bpawns = board.Pieces(Black, Pawn);

        if (!probed)
        {
            int wPawns = board.NumOfPieces(White, Pawn);
            int bPawns = board.NumOfPieces(Black, Pawn);
            updateScore(pawnStructure, PawnBonus[Opening]*wPawns, PawnBonus[EndGame]*wPawns);
            updateScore(pawnStructure, -PawnBonus[Opening]*bPawns, -PawnBonus[EndGame]*bPawns);

            Byte wpawnset = (Byte) Utils::BitBoard::SouthFill(wpawns);
            Byte bpawnset = (Byte) Utils::BitBoard::SouthFill(bpawns);

            int wislands = PopCount(wpawnset & (wpawnset ^ (wpawnset >> 1)));
            int bislands = PopCount(bpawnset & (bpawnset ^ (bpawnset >> 1)));

            updateScore(pawnStructure, -pawnIslandsP[Opening][wislands], -pawnIslandsP[EndGame][wislands]);
            updateScore(pawnStructure, pawnIslandsP[Opening][bislands], pawnIslandsP[EndGame][bislands]);

            for (File f = 0; f<8; f++)
            {
                int pawns;
                if ((pawns = board.PawnsOnFile(White, f)))
                {
                    if (!(wpawns & MoveDatabase::SideFiles[f]))
                        updateScore(pawnStructure, -isolatedPawnP[f]);

                    updateScore(pawnStructure, -multiPawnP[pawns]);

                    if (f < 7)
                        updateScore(pawnStructure, -multiPawnP[board.PawnsOnFile(White, ++f)]);
                    // since there's a pawn on this file, the next pawn (if present) will not be isolated
                }
            }

            for (File f = 0; f<8; f++)
            {
                int pawns;
                if ((pawns = board.PawnsOnFile(Black, f)))
                {
                    if (!(bpawns & MoveDatabase::SideFiles[f]))
                        updateScore(pawnStructure, isolatedPawnP[f]);

                    updateScore(pawnStructure, multiPawnP[pawns]);

                    if (f < 7)
                        updateScore(pawnStructure, multiPawnP[board.PawnsOnFile(Black, ++f)]);
                    // since there's a pawn on this file, the next pawn (if present) will not be isolated
                }
            }

            entry->attacks[White] = pawnAttacks[White] = 
                Pawn::GetAnyAttack(board.Pieces(White, PieceType::Pawn), White, Constants::Universe); // squares attacked by White pawns
            entry->attacks[Black] = pawnAttacks[Black] = 
                Pawn::GetAnyAttack(board.Pieces(Black, PieceType::Pawn), Black, Constants::Universe); // squares attacked by Black pawns
        }

        BitBoard pieces[2] = { // color
            board.Pieces(White) ^ board.Pieces(White, Pawn),
            board.Pieces(Black) ^ board.Pieces(Black, Pawn)
        };


        // loop through pawns (skipped when hash hit)
        for (Color c = White; !probed && c < PieceColor::None; c++)
        {
            BitBoard pawns = board.Pieces(c, Pawn);
            while(pawns != 0)
            {
                Napoleon::Square sq = BitScanForwardReset(pawns);
                updateScore(pawnStructure, evaluatePawn(c, sq, board));
            }
        }

        // pinned pieces penalty
        BitBoard pinned = board.PinnedPieces(); // TODO: may be sent by search
        updateScore(scores, -10*PopCount(pinned & board.Pieces(White)));
        updateScore(scores, 10*PopCount(pinned & board.Pieces(Black)));

        BitBoard unpinnedPawnAttacks[2] = { // color 
            Pawn::GetAnyAttack(board.Pieces(White, PieceType::Pawn) & ~pinned, White, Constants::Universe), // squares attacked by White pawns
            Pawn::GetAnyAttack(board.Pieces(Black, PieceType::Pawn) & ~pinned, Black, Constants::Universe) // squares attacked by Black pawns
        };

        // pseudo pawn-fork evaluation:
        // we check if multiple (not pinned) pawns, attack different pieces at ones 
        // (which includes forks)
        // TODO: adjust penalty depending on the forked piece
        BitBoard forked = 0;
        for(Color c = White; c < PieceColor::None; c++)
        {
            if(PopCount(forked = unpinnedPawnAttacks[c] & pieces[GetOpposite(c)]) >= 2)
            {
                assert(forked != 0 && forked >= 2);
                updateScore(scores, c == White ? 50 : -50);
            }
        }

        // pieces evaluation
        Piece piece;
        Color enemy;
        auto pieceList = board.PieceList();

        for (Napoleon::Square sq = IntA1; sq <= IntH8; sq++)
        {
            piece = pieceList[sq];

            if (piece.Type != PieceType::None && piece.Type != Pawn && piece.Type != King)
            {
                enemy = GetOpposite(piece.Color);
                updateScore(scores, EvaluatePiece(piece, sq, king_proximity[enemy], board)); // opponent's king proximity
            }
        }

        pawnTable.Save(board.pawnKey, pawnStructure);
        updateScore(scores, pawnStructure);

        // pseudo knight-forks evaluation
        /*
           for(Color c = White; c < PieceColor::None; c++)
           {
           Color enemy = GetOpposite(c);
           if(PopCount(unpinnedKnightAttacks[c] & 
           (board.Pieces(enemy, King) 
           | board.Pieces(enemy, Queen) 
           | board.Pieces(enemy, Rook))) >= 2)
           {
           updateScore(scores, c == White ? 20 : -20);
           }
           }
           */



        //KING SAFETY
        //king attacks table application (incrementally computed piece by piece)
        //TODO: try not to scale down
        updateScore(scores, kingAttacks[kingAttacksCount[White]], kingAttacks[kingAttacksCount[White]]/2);
        updateScore(scores, -kingAttacks[kingAttacksCount[Black]], -kingAttacks[kingAttacksCount[Black]]/2);

        //pawn shelter
        int shelter1 = 0, shelter2 = 0;

        if (kingFile[White] > Constants::Files::IntE)
        {
            shelter1 = PopCount(wpawns & WhiteKingShield);
            shelter2 = PopCount(wpawns & OneStepNorth(WhiteKingShield));
        }
        else if (kingFile[White] < Constants::Files::IntD)
        {
            shelter1 = PopCount(wpawns & WhiteQueenShield);
            shelter2 = PopCount(wpawns & OneStepNorth(WhiteQueenShield));
        }
        // else apply penalty

        updateScore(scores, shelter1 * 5.5 + shelter2 * 2, shelter1 + shelter2 ); // shielding bonus

        shelter1 = shelter2 = 0;
        if (kingFile[Black] > Constants::Files::IntE)
        {
            shelter1 = PopCount(bpawns & BlackKingShield);
            shelter2 = PopCount(bpawns & OneStepSouth(BlackKingShield));
        }
        else if (kingFile[Black] < Constants::Files::IntD)
        {
            shelter1 = PopCount(bpawns & BlackQueenShield);
            shelter2 = PopCount(bpawns & OneStepSouth(BlackQueenShield));
        }
        // else apply penalty

        updateScore(scores, -(shelter1 * 5.5 + shelter2 * 2), -shelter1 -shelter2); // shielding bonus


        /* DISABLED TO TEST MOBILITY VARIATION
        // HANGING PIECES:
        bool eval_hanging = hanging
        && attacks[White]
        && attacks[Black];

        if (eval_hanging)
        {
        attacks[White] |= pawnAttacks[White];
        attacks[Black] |= pawnAttacks[Black];
        }

        for (Color c = White; eval_hanging && c < PieceColor::None; c++)
        {
        for (Type piece = PieceType::Queen; piece > PieceType::Pawn; piece--)
        {
        BitBoard pieces = board.Pieces(c, piece);
        bool found = false;
        while(pieces)
        {
        auto sq = BitScanForwardReset(pieces);
        enemy = GetOpposite(c);
        if(attacks[enemy] & SquareMask[sq])
        {
        hangingValue[c] = PieceValue[piece];
        found = true;
        break;
        }
        }
        if (found) break;
        }
        }
        */

        //TODO: check whether the king is in castle position

        int opening = scores.first; // opening score
        int endgame = scores.second; // endgame score
        int phase = board.Phase(); // game phase (0-256)

        int score = ((opening * (Constants::Eval::MaxPhase - phase)) + (endgame * phase)) / Constants::Eval::MaxPhase; // linear-interpolated score

        return score * (1-(board.SideToMove()*2)); // score relative to side to move
    }


    Score Evaluation::EvaluatePiece(Piece piece, Square square, BitBoard king_proxy, Board& board)
    {
        using namespace Utils::BitBoard;
        using namespace Utils::Piece;
        using namespace Constants::Masks;
        using namespace Constants::Eval;

        Color us = piece.Color;
        Color enemy = GetOpposite(us);
        BitBoard b = 0;
        int tropism = 0;
        int distance = 7; // longest distance
        Square ksq = board.KingSquare(enemy); // enemy king
        Score bonus(0,0);
        File file;

        // TODO: do not count mobility if the piece is pinned
        switch(piece.Type)
        {
            case PieceType::Knight:
                b = Knight::TargetsFrom(square, us, board);

                //if (!(SquareMask[square] & pinned)) // only save unpinned knight attacks
                //unpinnedKnightAttacks[us] |= b; // used to evaluate knight forks

                tropism = 2; 
                distance = MoveDatabase::Distance[square][ksq];
                b &= ~pawnAttacks[enemy];
                break;

            case PieceType::Bishop:
                b = Bishop::TargetsFrom(square, us, board);
                tropism = 2; // TO TEST: divide by distance to king
                distance = MoveDatabase::Distance[square][ksq]*2;
                break;

            case PieceType::Rook:
                b = Rook::TargetsFrom(square, us, board);
                tropism = 4;
                distance = MoveDatabase::Distance[square][ksq]*3;
                file = Utils::Square::GetFileIndex(square);

                ///if(MoveDatabase::FrontSpan[us][square] & entry->passers[enemy])
                ///updateScore(bonus, supportedPassed[Opening], supportedPassed[EndGame]);

                if((FileMask[file] & board.Pieces(PieceType::Pawn)) == 0) // OPEN FILE
                    updateScore(bonus, 2*HalfOpenFileBonus[Opening], 2*HalfOpenFileBonus[EndGame]);

                else if((FileMask[file] & board.Pieces(us, PieceType::Pawn)) == 0) // HALF-OPEN FILE
                    updateScore(bonus, HalfOpenFileBonus[Opening], HalfOpenFileBonus[EndGame]);

                break;

            case PieceType::Queen:
                b = Queen::TargetsFrom(square, us, board);
                tropism = 6;
                distance = MoveDatabase::Distance[square][ksq]/2;
                break;

            default:
                return bonus;
        }

        //attacks[us] |= b;

        int count = PopCount(b);
        // We consider the number of attacks to the king zone, weighted by piece type.
        // We then add up (7 - distance to the king):
        kingAttacksCount[us] += tropism*PopCount(king_proxy & b) + (7 - distance); 

        updateScore(bonus, mobilityBonus[Opening][piece.Type][count], 
                mobilityBonus[EndGame][piece.Type][count]);

        if (piece.Color == PieceColor::White) return bonus;
        else return -bonus;
    }

    void Evaluation::formatParam(std::string name, int wvalue, int bvalue)
    {
        std::cout << name << ":\t\t " << wvalue << " \t\t " << bvalue << std::endl;
    }

    void Evaluation::formatParam(std::string name, Score wvalue, Score bvalue, int phase)
    {
        int w = interpolate(wvalue, phase), b = interpolate(bvalue, phase);
        formatParam(name, w, b);
    }

    void Evaluation::PrintEval(Board& board)
    {
        using namespace PieceColor;
        using namespace Constants::Squares;
        using namespace Constants::Eval;
        using namespace Constants::Castle;
        using namespace Constants::Masks;
        using namespace Utils::BitBoard;
        using namespace CompassRose;
        using namespace std;

        int phase = board.Phase();
        Score scores(0, 0);
        Score wPstValues, bPstValues;

        cout << "\t EVALUATION " << endl;
        cout << "\t\t WHITE \t\t BLACK" << endl;

        formatParam("material", board.Material(White), board.Material(Black));
        formatParam("mat_balance", 
                board.MaterialBalance(White), 
                board.MaterialBalance(Black));
        formatParam("pst", 
                interpolate(board.PstValue(White), phase), 
                -interpolate(board.PstValue(Black), phase));

        Score pawn_bonus = Score(PawnBonus[Opening], PawnBonus[EndGame]);
        Score knight_bonus = Score(KnightBonus[Opening], KnightBonus[EndGame]);
        Score rook_bonus = Score(RookBonus[Opening], RookBonus[EndGame]);
        Score w_pqueen = Score(0, 0), b_pqueen = Score(0, 0); // premature queen development

        if (!board.IsOnSquare(White, PieceType::Queen, IntD1))
            w_pqueen = Score(-15, 0);

        if (!board.IsOnSquare(Black, PieceType::Queen, IntD8))
            b_pqueen = Score(15, 0);

        Score w_bishop_pair = Score(BishopPair[Opening], BishopPair[EndGame]);
        Score b_bishop_pair = Score(-BishopPair[Opening], -BishopPair[EndGame]);

        auto wking_square = board.KingSquare(White);
        auto bking_square = board.KingSquare(Black);

        BitBoard wpawns = board.Pieces(White, Pawn);
        BitBoard bpawns = board.Pieces(Black, Pawn);

        BitBoard king_proximity[2] = { // color
            MoveDatabase::KingProximity[White][wking_square],
            MoveDatabase::KingProximity[Black][bking_square]
        };

        formatParam("pawn-bonus", interpolate(pawn_bonus, phase), -interpolate(pawn_bonus, phase));
        formatParam("knight-bonus", interpolate(knight_bonus, phase), -interpolate(knight_bonus, phase));
        formatParam("rook-bonus", interpolate(rook_bonus, phase), -interpolate(rook_bonus, phase));
        formatParam("prem. queen", w_pqueen, b_pqueen, phase);
        formatParam("tempo", board.SideToMove() == White ? 5 : -5, board.SideToMove() == White ? 0 : -5);
        formatParam("bish. pair", w_bishop_pair, b_bishop_pair, phase);

        int shelter1 = 0, shelter2 = 0;

        //pawn shelter
        if (SquareMask[wking_square] & WhiteKingSide)
        {
            shelter1 = PopCount(wpawns & WhiteKingShield);
            shelter2 = PopCount(wpawns & OneStepNorth(WhiteKingShield));
        }
        else if (SquareMask[wking_square] & WhiteQueenSide)
        {
            shelter1 = PopCount(wpawns & WhiteQueenShield);
            shelter2 = PopCount(wpawns & OneStepNorth(WhiteQueenShield));
        }
        // else apply penalty

        int bshelter1 = 0, bshelter2 = 0;

        if (SquareMask[bking_square] & BlackKingSide)
        {
            bshelter1 = PopCount(bpawns & BlackKingShield);
            bshelter2 = PopCount(bpawns & OneStepSouth(BlackKingShield));
        }
        else if (SquareMask[bking_square] & BlackQueenSide)
        {
            bshelter1 = PopCount(bpawns & BlackQueenShield);
            bshelter2 = PopCount(bpawns & OneStepSouth(BlackQueenShield));
        }

        formatParam("king shield", 
                Score(shelter1*4+shelter2*2, shelter1+shelter2),
                Score(-(bshelter1*4 + bshelter2*2), -bshelter1 -bshelter2),
                phase);
    }


}

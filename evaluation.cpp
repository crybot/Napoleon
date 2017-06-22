#include "evaluation.h"
#include "piece.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"
#include "queen.h"

namespace Napoleon
{
    int Evaluation::multiPawnP[8] = { 0, 0, 10, 20, 35, 50, 75, 100 };
    int Evaluation::isolatedPawnP[8] = { 5, 7, 10, 18, 18, 10, 7, 5 };
    int Evaluation::passedPawn[3][8] = // phase, rank
    {
        {0, 0, 10, 15, 25, 30, 30, 0}, // OPENING
        {0},                        // MIDDLEGAME (not used)
        {0, 0, 10, 20, 40, 60, 125, 0} // ENDGAME
    };

    int Evaluation::mobilityBonus[][Constants::QueenMaxMoves + 1] =
    {
        {}, // PAWNS
        {-15, -10, -5, 0, 5, 10, 10, 15, 15}, // KNIGHTS
        {-15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 30, 35, 35, 35}, // BISHOPS
        {-5, -5, 0, 5, 10, 10, 15, 20, 30, 35, 35, 40, 40, 40, 40}, // ROOKS
        {-5, -4, -3, -2, -1, 0, 5, 10,
         13, 16, 18, 20, 22, 24, 26, 28,
         29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30}, // QUEENS
        {}, // KING
        {}, // NONE
    };

    int Evaluation::Evaluate(Board& board)
    {
        using namespace PieceColor;
        using namespace Constants::Squares;
        using namespace Constants::Eval;
        using namespace Constants::Castle;
        using namespace Constants::Masks;
        using namespace Utils::BitBoard;
        using namespace Utils::Piece;
        using namespace CompassRose;

        Score scores(0, 0);
        Score wPstValues, bPstValues;
        auto wking_square = board.KingSquare(White);
        auto bking_square = board.KingSquare(Black);

        BitBoard king_proximity[2] = { // color
            MoveDatabase::KingProximity[White][wking_square],
            MoveDatabase::KingProximity[Black][bking_square]
        };

        // material evaluation
        int material = board.MaterialBalance(White);

        // Piece Square Value evaluation
        wPstValues = board.PstValue(White);
        bPstValues = board.PstValue(Black);

        updateScore(scores, material + (wPstValues.first - bPstValues.first)/2, material + (wPstValues.second - bPstValues.second)/2);

        /* PHASE-DEPENDENT piece bonus*/
        int wPawns = board.NumOfPieces(White, Pawn);
        int bPawns = board.NumOfPieces(Black, Pawn);
        updateScore(scores, PawnBonus[Opening]*wPawns, PawnBonus[EndGame]*wPawns);
        updateScore(scores, -PawnBonus[Opening]*bPawns, -PawnBonus[EndGame]*bPawns);

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


        /* PAWN STRUCTURE */    
        // doubled/isolated pawns evaluation

        BitBoard wpawns = board.Pieces(White, Pawn);
        BitBoard bpawns = board.Pieces(Black, Pawn);

        for (File f = 0; f<8; f++)
        {
            int pawns;
            if ((pawns = board.PawnsOnFile(White, f)))
            {
                if (!(wpawns & MoveDatabase::SideFiles[f]))
                    updateScore(scores, -isolatedPawnP[f]);

                updateScore(scores, -multiPawnP[pawns]);

                if (f < 7)
                    updateScore(scores, -multiPawnP[board.PawnsOnFile(White, ++f)]);
                // since there's a pawn on this file, the next pawn (if present) would not be isolated
            }
        }
        for (File f = 0; f<8; f++)
        {
            int pawns;
            if ((pawns = board.PawnsOnFile(Black, f)))
            {
                if (!(bpawns & MoveDatabase::SideFiles[f]))
                    updateScore(scores, isolatedPawnP[f]);

                updateScore(scores, multiPawnP[pawns]);

                if (f < 7)
                    updateScore(scores, multiPawnP[board.PawnsOnFile(Black, ++f)]);
                // since there's a pawn on this file, the next pawn (if present) would not be isolated
            }
        }

        // mobility evaluation
        Piece piece;
        auto pieceList = board.PieceList();

        for (Napoleon::Square sq = IntA1; sq <= IntH8; sq++)
        {
            piece = pieceList[sq];

            if (piece.Type != PieceType::None)
            {
                if (piece.Type == Pawn)
                {
                    if ((MoveDatabase::PasserSpan[piece.Color][sq] & board.Pieces(GetOpposite(piece.Color), Pawn)) == 0)
                    {
                        auto rank = Utils::Square::GetRankIndex(sq);
                        if (piece.Color == White)
                            updateScore(scores, passedPawn[Opening][rank], passedPawn[EndGame][rank]);
                        else
                            updateScore(scores, -passedPawn[Opening][7 - rank], -passedPawn[EndGame][7 - rank]);
                    }
                }
                else if (piece.Color == White)
                    updateScore(scores, EvaluatePiece(piece, sq, king_proximity[Black], board)); // opponent's king's proximity
                else
                    updateScore(scores, -EvaluatePiece(piece, sq, king_proximity[White], board)); // opponent's king's proximity
            }
        }

        //KING SAFETY

        int shelter1 = 0, shelter2 = 0;

        //pawn shelter
        BitBoard pawns = board.Pieces(White, Pawn);
        if (SquareMask[wking_square] & WhiteKingSide)
        {
            shelter1 = PopCount(pawns & WhiteKingShield);
            shelter2 = PopCount(pawns & OneStepNorth(WhiteKingShield));
        }
        else if (SquareMask[wking_square] & WhiteQueenSide)
        {
            shelter1 = PopCount(pawns & WhiteQueenShield);
            shelter2 = PopCount(pawns & OneStepNorth(WhiteQueenShield));
        }
        // else apply penalty

        updateScore(scores, shelter1 * 4 + shelter2 * 3, shelter1 * 2 + shelter2 * 3); // shielding bonus
        
        pawns = board.Pieces(Black, Pawn);
        if (SquareMask[bking_square] & BlackKingSide)
        {
            shelter1 = PopCount(pawns & BlackKingShield);
            shelter2 = PopCount(pawns & OneStepSouth(BlackKingShield));
        }
        else if (SquareMask[bking_square] & BlackQueenSide)
        {
            shelter1 = PopCount(pawns & BlackQueenShield);
            shelter2 = PopCount(pawns & OneStepSouth(BlackQueenShield));
        }
        // else apply penalty

        updateScore(scores, -(shelter1 * 4 + shelter2 * 3), -(shelter1 * 2 + shelter2 * 3)); // shielding bonus


        //TODO: check wheter the king is in castle position

        int opening = scores.first; // opening score
        int endgame = scores.second; // endgame score
        int phase = board.Phase(); // game phase (0-256)

        int score = ((opening * (Constants::Eval::MaxPhase - phase)) + (endgame * phase)) / Constants::Eval::MaxPhase; // linear-interpolated score

        return score * (1-(board.SideToMove()*2)); // score relative to side to move
    }

    int Evaluation::EvaluatePiece(Piece piece, Square square, BitBoard king_proxy, Board& board)
    {
        using namespace Utils::BitBoard;
        using namespace Utils::Piece;

        Color us = piece.Color;
        Color enemy = GetOpposite(us);
        BitBoard b = 0;
        int tropism = 0;
        int distance = 7; // longest distance
        Square ksq = board.KingSquare(enemy); // enemy king

        switch(piece.Type)
        {
            case PieceType::Knight:
                b = Knight::TargetsFrom(square, us, board) &
                    ~Pawn::GetAnyAttack(board.Pieces(enemy, PieceType::Pawn), enemy, Constants::Universe); // exclude squares controlled by enemy pawns
                tropism = 5; 
                distance = MoveDatabase::Distance[square][ksq];
                break;

            case PieceType::Bishop:
                b = Bishop::TargetsFrom(square, us, board);
                tropism = 10; // TO TEST: divide by distance to king
                distance = MoveDatabase::Distance[square][ksq]*2;
                break;

            case PieceType::Rook:
                b = Rook::TargetsFrom(square, us, board);
                tropism = 10;
                distance = MoveDatabase::Distance[square][ksq]*2;
                break;

            case PieceType::Queen:
                b = Queen::TargetsFrom(square, us, board);
                tropism = 3;
                distance = MoveDatabase::Distance[square][ksq]/2;
                break;
        }

        int count = PopCount(b);
        assert(count <= Constants::QueenMaxMoves);

        tropism = tropism*PopCount(king_proxy & b) + (7 - distance);
        tropism = ((float)tropism*board.Material(us))/(float)Constants::Eval::MaxPlayerMat; // tropism scaling

        return mobilityBonus[piece.Type][count] + tropism;
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

        BitBoard king_proximity[2] = { // color
            MoveDatabase::KingProximity[White][board.KingSquare(White)],
            MoveDatabase::KingProximity[Black][board.KingSquare(Black)]
        };

        formatParam("pawn-bonus", interpolate(pawn_bonus, phase), -interpolate(pawn_bonus, phase));
        formatParam("knight-bonus", interpolate(knight_bonus, phase), -interpolate(knight_bonus, phase));
        formatParam("rook-bonus", interpolate(rook_bonus, phase), -interpolate(rook_bonus, phase));
        formatParam("prem. queen", w_pqueen, b_pqueen, phase);
        formatParam("tempo", board.SideToMove() == White ? 5 : -5, board.SideToMove() == White ? 0 : -5);
        formatParam("bish. pair", w_bishop_pair, b_bishop_pair, phase);
        cout << endl;

        Display(king_proximity[White]);
        cout << endl;
        Display(king_proximity[Black]);
        cout << endl << endl;
    }


}

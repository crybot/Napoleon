#include "evaluation.h"
#include "piece.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"
#include "queen.h"
#include "board.h"

namespace Napoleon
{
    int Evaluation::multiPawnP[8] = { 0, 0, 22, 50, 100, 100, 100, 100 };
    int Evaluation::isolatedPawnP[8] = { 5, 8, 12, 17, 17, 12, 8, 5 };

    int Evaluation::mobilityBonus[][Constants::QueenMaxMoves + 1] =
    {
        {}, // PAWNS
        {-30, -16, -5, 1, 12, 18, 23, 26, 28}, // KNIGHTS
        {-25, -15, 0, 10, 25, 37, 47, 57, 65, 70, 74, 76, 78, 80}, // BISHOPS
        {-15, 0, 15, 30, 42, 52, 60, 68, 74, 80, 86, 91, 95, 99, 101}, // ROOKS
        {-10, -9, -7, -5, -2, 1, 7, 10,
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

        Score scores(0, 0);
        Score wPstValues, bPstValues;
        
        // material evaluation
        int material = board.MaterialBalance(White);
        
        // Piece Square Value evaluation
        wPstValues = board.PstValue(White);
        bPstValues = board.PstValue(Black);
        
        updateScore(scores, material + (wPstValues.first - bPstValues.first), material + (wPstValues.second - bPstValues.second));
        
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
            updateScore(scores, 15, 0);
        
        if (!board.IsOnSquare(Black, PieceType::Queen, IntD8))
            updateScore(scores, -15, 0);
        
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

        int pawnsOnLeftFile[2] = {0};
        int pawnsOnFile[2] = {0};

        for (File f = 0; f<8; f++)
        {
            pawnsOnFile[White] = board.PawnsOnFile(White, f);
            pawnsOnFile[Black] = board.PawnsOnFile(Black, f);

            if (f < 7)
            {
                if (board.PawnsOnFile(White, f+1) == 0 && pawnsOnLeftFile[White] == 0)
                    updateScore(scores, -isolatedPawnP[f]);

                if (board.PawnsOnFile(Black, f+1) == 0 && pawnsOnLeftFile[Black] == 0)
                    updateScore(scores, isolatedPawnP[f]);
            }
            else
            {
                if (pawnsOnLeftFile[White] == 0)
                    updateScore(scores, -isolatedPawnP[f]);

                if (pawnsOnLeftFile[Black] == 0)
                    updateScore(scores, isolatedPawnP[f]);
            }

            pawnsOnLeftFile[White] = pawnsOnFile[White];
            pawnsOnLeftFile[Black] = pawnsOnFile[Black];

            updateScore(scores, -multiPawnP[pawnsOnFile[White]]);
            updateScore(scores, multiPawnP[pawnsOnFile[Black]]);
        }

        // mobility evaluation
        Piece piece;
        auto pieceList = board.PieceList();

        for (Napoleon::Square sq = IntA1; sq <= IntH8; sq++)
        {
            piece = pieceList[sq];

            if (piece.Type != PieceType::None)
            {
                if (piece.Color == White)
                    updateScore(scores, EvaluatePiece(piece, sq, board));
                else
                    updateScore(scores, -EvaluatePiece(piece, sq, board));
            }
        }

        int opening = scores.first; // opening score
        int endgame = scores.second; // endgame score
        int phase = board.Phase(); // game phase (0-256)

        int score = ((opening * (Constants::Eval::MaxPhase - phase)) + (endgame * phase)) / Constants::Eval::MaxPhase; // linear-interpolated score

        return score * (1-(board.SideToMove()*2)); // score relative to side to move
    }
    
    int Evaluation::EvaluatePiece(Piece piece, Square square, Board& board)
    {
        using namespace Utils::BitBoard;
        using namespace Utils::Piece;

        Color us = piece.Color;
        Color enemy = GetOpposite(us);
        BitBoard b = 0;
        
        switch(piece.Type)
        {
        case PieceType::Knight:
            b = Knight::TargetsFrom(square, us, board) &
                    ~Pawn::GetAnyAttack(board.Pieces(enemy, PieceType::Pawn), enemy, Constants::Universe); // exclude squares controlled by enemy pawns
            break;
            
        case PieceType::Bishop:
            b = Bishop::TargetsFrom(square, us, board);
            break;
            
        case PieceType::Rook:
            b = Rook::TargetsFrom(square, us, board);
            break;

        case PieceType::Queen:
            b = Queen::TargetsFrom(square, us, board);
            break;
        }

        int count = PopCount(b);
        assert(count <= Constants::QueenMaxMoves);

        return mobilityBonus[piece.Type][count];
    }
    
}

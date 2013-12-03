#include "evaluation.h"
#include "utils.h"
#include "board.h"
#include "piece.h"
#include "rook.h"
#include "bishop.h"

namespace Napoleon
{
    int Evaluation::pawnSquareValue[64] =
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 15,-20,-20, 15, 10,  5,
        5, -5,-10,  0, 0,-10, -5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5,  5, 10, 25, 25, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    int Evaluation::knightSquareValue[64] =
    {
        -50,-20,-30,-30,-30,-30,-20,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };

    int Evaluation::bishopSquareValue[64] =
    {

        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 20, 10, 10, 20,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };

    int Evaluation::rookSquareValue[64] =
    {
        0,  0,  0,  5,  5,  0,  0,  0,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        5,  10, 20, 20, 20, 20, 10, 5,
        0,   0,  0,  0,  0,  0,  0, 0
    };

    int Evaluation::queenSquareValue[64] =
    {
        -20, -5, -5,  0,  0, -5, -5, -20,
        -10,  0,  0,  0,  0,  0,  0, -10,
        -5,   0,  5,  5,  5,  5,  0,  -5,
        -5,   0,  5,  5,  5,  5,  0,  -5,
        -5,   0,  5,  5,  5,  5,  0,  -5,
        -5,   0,  0,  0,  0,  0,  0,  -5,
        0,   5, 10, 10, 10, 10,  5,   0,
        0,   0,  0,  0,  0,  0,  0,   0
    };

    int Evaluation::kingMiddleGame[64] =
    {
        20, 30, 10,  0,  0, 10, 35, 20,
        20, 10,  0,  0,  0,  0, 10, 20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30
    };

    int Evaluation::kingEndGame[64] =
    {
        -50,-30,-30,-30,-30,-30,-30,-50
        -30,-30,  0,  0,  0,  0,-30,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -50,-40,-30,-20,-20,-30,-40,-50
    };

    int Evaluation::multiPawnP[8] = { 0, 0, 20, 50, 100, 100, 100, 100 };

    int Evaluation::Evaluate(Board & board)
    {
        using namespace Utils::BitBoard;
        using namespace PieceColor;
        using namespace Constants::Squares;

        int score = 0;

        // material evaluation
        int material = board.Material(White) - board.Material(Black);

        // Piece Square Value evaluation
        int wPST = board.PstValue(White);
        int bPST = board.PstValue(Black);

        score += material + (wPST - bPST);

        // premature queen development
        if (!board.IsOnSquare(White, PieceType::Queen, IntD1))
            if (board.Material(White) > Constants::Eval::MiddleGameMat)
                score -= 10;

        if (!board.IsOnSquare(Black, PieceType::Queen, IntD8))
            if (board.Material(Black) > Constants::Eval::MiddleGameMat)
                score += 10;

        // tempo bonus
        if (board.SideToMove() == White)
            score += 5;
        else
            score -= 5;

        /* PAWN STRUCTURE */

        // doubled/tripled pawns evaluation
        for (File f = 0; f<8; f++)
        {
            score -= multiPawnP[board.PawnsOnFile(White, f)];
            score += multiPawnP[board.PawnsOnFile(Black, f)];
        }

        // mobility evaluation
        Piece piece;
        for (Napoleon::Square sq = 0; sq<64; sq++)
        {
            piece = board.GetPieceOnSquare(sq);
            if (piece.Type != PieceType::None)
            {
                if (piece.Color == White)
                    score += EvaluatePiece(piece, sq, board);
                else
                    score -= EvaluatePiece(piece, sq, board);
            }
        }

        //        // king safety

        //        BitBoard occupiedSquares = board.OccupiedSquares;
        //        BitBoard targets = Constants::Empty;

        //        int kingSquare = board.KingSquare(board.SideToMove());

        //        targets |= MoveDatabase::GetRankAttacks(occupiedSquares, kingSquare);
        //        targets |= MoveDatabase::GetFileAttacks(occupiedSquares, kingSquare);
        //        targets |= MoveDatabase::GetA1H8DiagonalAttacks(occupiedSquares, kingSquare);
        //        targets |= MoveDatabase::GetH1A8DiagonalAttacks(occupiedSquares, kingSquare);

        //        score -= Utils::BitBoard::PopCount(targets & ~board.GetPieces(board.SideToMove()));

        //        targets = Constants::Empty;
        //        kingSquare = board.KingSquare(Utils::Piece::GetOpposite(board.SideToMove()));

        //        targets |= MoveDatabase::GetRankAttacks(occupiedSquares, kingSquare);
        //        targets |= MoveDatabase::GetFileAttacks(occupiedSquares, kingSquare);
        //        targets |= MoveDatabase::GetA1H8DiagonalAttacks(occupiedSquares, kingSquare);
        //        targets |= MoveDatabase::GetH1A8DiagonalAttacks(occupiedSquares, kingSquare);

        //        score += Utils::BitBoard::PopCount(targets & ~board.GetPieces(Utils::Piece::GetOpposite(board.SideToMove())));

        return score * (1-(board.SideToMove()*2));
    }

    int Evaluation::EvaluatePiece(Piece piece, Square square, Board& board)
    {
        using namespace Utils::BitBoard;
        BitBoard b = 0;

        switch(piece.Type)
        {
        case PieceType::Knight:
            b = MoveDatabase::KnightAttacks[square] & ~board.GetPieces(piece.Color);
            return 0.3*PopCount(b);

        case PieceType::Bishop:
            b = (MoveDatabase::GetA1H8DiagonalAttacks(board.OccupiedSquares, square)
                 | MoveDatabase::GetH1A8DiagonalAttacks(board.OccupiedSquares, square))
                    & ~board.GetPieces(piece.Color);

            return 0.5*PopCount(b);

        case PieceType::Rook:
            b = (MoveDatabase::GetFileAttacks(board.OccupiedSquares, square)
                 | MoveDatabase::GetRankAttacks(board.OccupiedSquares, square))
                    & ~board.GetPieces(piece.Color);

            return 0.1*PopCount(b);

        default:
            return 0;
        }
    }

    int Evaluation::CalculatePST(Piece piece, Square square, Board& board)
    {
        using namespace Utils::Square;
        using namespace PieceColor;
        using namespace Constants::Squares;

        switch(piece.Type)
        {
        case PieceType::Pawn:
            return piece.Color ==  White ? pawnSquareValue[square] : pawnSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::Knight:
            return piece.Color == White ? knightSquareValue[square] : knightSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::Bishop:
            return piece.Color == White ? bishopSquareValue[square] : bishopSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::Rook:
            return piece.Color == White ? rookSquareValue[square] : rookSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::Queen:
            return piece.Color == White ? queenSquareValue[square] : queenSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::King:
            if (board.Material(piece.Color) > Constants::Eval::EndGameMat)
                return piece.Color == White ? kingMiddleGame[square] : kingMiddleGame[GetSquareIndex(square % 8, 7 - (square/8))];
            else
                return piece.Color == White ? kingEndGame[square] : kingEndGame[GetSquareIndex(square % 8, 7 - (square/8))];
        }

        return 0;
    }
}

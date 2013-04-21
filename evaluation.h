#ifndef EVALUATION_H
#define EVALUATION_H
#include "defines.h"
#include "piece.h"
#include "utils.h"
#include "board.h"
namespace Napoleon
{
    extern int pawnSquareValue[64];
    extern int knightSquareValue[64];
    extern int bishopSquareValue[64];
    extern int queenSquareValue[64];
    extern int kingSquareValue[64];

    class Evaluation
    {
    public:
        static int Evaluate(Board&);

    private:
        static int evaluatePiece(Byte, Byte, int);
        template<Byte>
        static int evaluateMobility(Board&, BitBoard);

    };


    INLINE int Evaluation::Evaluate(Board & board)
    {
        using namespace Utils::BitBoard;

        int material =
                +   2000*(board.NumOfPieces[PieceColor::White][PieceType::King] - board.NumOfPieces[PieceColor::Black][PieceType::King])
                +   900*(board.NumOfPieces[PieceColor::White][PieceType::Queen] - board.NumOfPieces[PieceColor::Black][PieceType::Queen])
                +   500*(board.NumOfPieces[PieceColor::White][PieceType::Rook] - board.NumOfPieces[PieceColor::Black][PieceType::Rook])
                +   330*(board.NumOfPieces[PieceColor::White][PieceType::Bishop] - board.NumOfPieces[PieceColor::Black][PieceType::Bishop])
                +   320*(board.NumOfPieces[PieceColor::White][PieceType::Knight] - board.NumOfPieces[PieceColor::Black][PieceType::Knight])
                +   100*(board.NumOfPieces[PieceColor::White][PieceType::Pawn] - board.NumOfPieces[PieceColor::Black][PieceType::Pawn]);


        int wM = 0;
        int bM = 0;

        for (int i=0; i<64; i++)
        {
            if (board.PieceSet[i].Color != PieceColor::None)
            {
                Piece piece = board.PieceSet[i];

                if(board.PieceSet[i].Color == PieceColor::White)
                {
                    if (board.PieceSet[i].Type == PieceType::Queen)
                    {
                        if (PopCount(board.OccupiedSquares) > 28)
                            wM += queenSquareValue[i];
                    }
                    else
                        wM += evaluatePiece(piece.Type, piece.Color, i);
                }
                else
                {
                    if (board.PieceSet[i].Type == PieceType::Queen)
                    {
                        if (PopCount(board.OccupiedSquares) > 28)
                            bM += queenSquareValue[Utils::Square::GetSquareIndex(i % 8, 7 - (i/8))];
                    }
                    else
                        bM += evaluatePiece(piece.Type, piece.Color, i);
                }
            }
        }

        return (material + (wM - bM)) * (1-(board.SideToMove*2));
    }

    INLINE int Evaluation::evaluatePiece(Byte piece, Byte color, int square)
    {
        using namespace Utils::Square;
        switch(piece)
        {
        case PieceType::Pawn:
            return color == PieceColor::White ? pawnSquareValue[square] : pawnSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::Knight:
            return color == PieceColor::White ? knightSquareValue[square] : knightSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::Bishop:
            return color == PieceColor::White ? bishopSquareValue[square] : bishopSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        case PieceType::King:
            return color == PieceColor::White ? kingSquareValue[square] : kingSquareValue[GetSquareIndex(square % 8, 7 - (square/8))];

        }

        return 0;
    }
}

#endif // EVALUATION_H

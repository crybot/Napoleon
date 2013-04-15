#include "evaluation.h"
#include "utils.h"
#include "board.h"
#include "piece.h"

namespace Napoleon
{
    int pawnSquareValue[64] =
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

    int knightSquareValue[64]
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

    int bishopSquareValue[64]
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

    int queenSquareValue[64]
    {
        -10,-10,-10, 0, -10,-10,-10,-10,
        -10,-10,-10,-10,-10,-10,-10,-10,
        -10,-10,-10,-10,-10,-10,-10,-10,
        -10,-10,-10,-10,-10,-10,-10,-10,
        -10,-10,-10,-10,-10,-10,-10,-10,
        -10,-10,-10,-10,-10,-10,-10,-10,
        -10,-10,-10,-10,-10,-10,-10,-10,
        -10,-10,-10,-10,-10,-10,-10,-10
    };

    int kingSquareValue[64]
    {
         20, 30, 50,  0,  0, 10, 60, 20,
         20, 20,  0,  0,  0,  0, 20, 20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30
    };





    template<Byte piece>
    int Evaluation::evaluateMobility(Board& board, BitBoard pieces)
    {
        using namespace Utils::BitBoard;
        int square;
        BitBoard b = 0;

        switch(piece)
        {
        case PieceType::Bishop:
            while (pieces)
            {
                square = BitScanForwardReset(pieces);

                b |= MoveDatabase::GetA1H8DiagonalAttacks(board.OccupiedSquares, square)
                        | MoveDatabase::GetH1A8DiagonalAttacks(board.OccupiedSquares, square);
            }

            return 0.5*PopCount(b);

        case PieceType::Queen:
            while(pieces)
            {
                square = BitScanForwardReset(pieces);

                b |= MoveDatabase::GetA1H8DiagonalAttacks(board.OccupiedSquares, square)
                        | MoveDatabase::GetH1A8DiagonalAttacks(board.OccupiedSquares, square)
                        | MoveDatabase::GetFileAttacks(board.OccupiedSquares, square)
                        | MoveDatabase::GetRankAttacks(board.OccupiedSquares, square);
            }
            return 0.01*PopCount(b);

        default:
            break;
        }

        return 0;
    }
}

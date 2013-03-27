#ifndef BOARD_H
#define BOARD_H
#include "constants.h"
#include "piece.h"

namespace Napoleon
{
    class Board
    {
    public:
        bool WhiteCanCastleOO;
        bool WhiteCanCastleOOO;
        bool BlackCanCastleOO;
        bool BlackCanCastleOOO;
        int EnPassantSquare;
        char SideToMove;

        Piece pieceSet[64];

        BitBoard WhitePieces;
        BitBoard BlackPieces;
        BitBoard OccupiedSquares;
        BitBoard EmptySquares;

        Board();
        void AddPiece(Piece, Square);
        void Equip();
        void Display();

    private:
        int kingSquare[2]; // by color
        BitBoard bitBoardSet[2][6] = { { Constants::Empty } };

        void initializePieceSet();
        void initializeCastlingStatus();
        void initializeSideToMove();
        void initializeEnPassantSquare();
        void initializeBitBoards();
        void clearPieceSet();
        void updateGenericBitBoards();

    };
}

#endif // BOARD_H

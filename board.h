#ifndef BOARD_H
#define BOARD_H
#include "constants.h"

namespace Napoleon
{
    class FenString;
    class Board
    {
    public:
        bool WhiteCanCastleOO;
        bool WhiteCanCastleOOO;
        bool BlackCanCastleOO;
        bool BlackCanCastleOOO;
        int EnPassantSquare;
        Byte SideToMove;

        Piece pieceSet[64];

        BitBoard WhitePieces;
        BitBoard BlackPieces;
        BitBoard OccupiedSquares;
        BitBoard EmptySquares;

        Board();
        void Equip();
        void Display() const;
        void AddPiece(Piece, Square);
        BitBoard GetPlayerPieces() const;
        BitBoard GetEnemyPieces() const;
        BitBoard GetPieceSet(Byte, Byte) const;
        void LoadGame(const FenString&);

    private:
        int kingSquare[2]; // color
        BitBoard bitBoardSet[2][6] = { { Constants::Empty } }; // color, type

        void clearPieceSet();
        void updateGenericBitBoards();
        void initializePieceSet();
        void initializeCastlingStatus();
        void initializeSideToMove();
        void initializeEnPassantSquare();
        void initializeBitBoards();
        void initializeBitBoards(const FenString&);
        void initializeSideToMove(const FenString&);
        void initializeCastlingStatus(const FenString&);
        void initializeEnPassantSquare(const FenString&);
        void initializePieceSet(const FenString&);

    };

    __always_inline BitBoard Board::GetPlayerPieces() const
    {
        return SideToMove == PieceColor::White ? WhitePieces : BlackPieces;
    }

    __always_inline BitBoard Board::GetEnemyPieces() const
    {
        return SideToMove == PieceColor::White ? BlackPieces : WhitePieces;
    }

    __always_inline BitBoard Board::GetPieceSet(Byte pieceColor, Byte pieceType) const
    {
        return bitBoardSet[pieceColor][pieceType];
    }
}

#endif // BOARD_H

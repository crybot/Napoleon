#include "board.h"
#include "utils.h"
#include "console.h"
#include "movedatabase.h"
#include "piece.h"
#include <iostream>

namespace Napoleon
{
    Board::Board()
    {
        MoveDatabase::InitAttacks();

        WhitePieces = Constants::Empty;
        BlackPieces = Constants::Empty;
        OccupiedSquares = Constants::Empty;
        EmptySquares = Constants::Empty;
    }

    void Board::Equip()
    {
        initializePieceSet();
        initializeCastlingStatus();
        initializeSideToMove();
        initializeEnPassantSquare();
        initializeBitBoards();
    }

    void Board::AddPiece(Piece piece, Square sq)
    {
        pieceSet[sq] = piece;
    }

    void Board::initializePieceSet()
    {
        clearPieceSet();

        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 8);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 9);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 10);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 11);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 12);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 13);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 14);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 15);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 48);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 49);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 50);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 51);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 52);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 53);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 54);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 55);

        /*TORRI*/
        AddPiece(Piece(PieceColor::White, PieceType::Rook), 0);
        AddPiece(Piece(PieceColor::White, PieceType::Rook), 7);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Rook), 56);
        AddPiece(Piece(PieceColor::Black, PieceType::Rook), 63);

        /*CAVALLI*/
        AddPiece(Piece(PieceColor::White, PieceType::Knight), 1);
        AddPiece(Piece(PieceColor::White, PieceType::Knight), 6);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Knight), 57);
        AddPiece(Piece(PieceColor::Black, PieceType::Knight), 62);

        /*ALFIERI*/
        AddPiece(Piece(PieceColor::White, PieceType::Bishop), 2);
        AddPiece(Piece(PieceColor::White, PieceType::Bishop), 5);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Bishop), 58);
        AddPiece(Piece(PieceColor::Black, PieceType::Bishop), 61);

        /*RE*/
        AddPiece(Piece(PieceColor::White, PieceType::King), 4);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::King), 60);

        /*REGINE*/
        AddPiece(Piece(PieceColor::White, PieceType::Queen), 3);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Queen), 59);
    }

    void Board::clearPieceSet()
    {
        for (int i=0; i<64; i++)
        {
            pieceSet[i] = Piece(PieceColor::None, PieceType::None);
        }
    }

    void Board::initializeCastlingStatus()
    {
        WhiteCanCastleOO = true;
        WhiteCanCastleOOO = true;
        BlackCanCastleOO = true;
        BlackCanCastleOOO = true;
    }

    void Board::initializeSideToMove()
    {
        SideToMove = PieceColor::White;
    }

    void Board::initializeEnPassantSquare()
    {
        EnPassantSquare = Constants::Squares::Invalid;
    }

    void Board::initializeBitBoards()
    {
        kingSquare[PieceColor::White] = 4;
        kingSquare[PieceColor::Black] = 60;

        bitBoardSet[PieceColor::White][PieceType::Pawn] = Constants::InitialPositions::WhitePawns;
        bitBoardSet[PieceColor::White][PieceType::Knight] = Constants::InitialPositions::WhiteKnights;
        bitBoardSet[PieceColor::White][PieceType::Bishop] = Constants::InitialPositions::WhiteBishops;
        bitBoardSet[PieceColor::White][PieceType::Rook] = Constants::InitialPositions::WhiteRooks;
        bitBoardSet[PieceColor::White][PieceType::Queen] = Constants::InitialPositions::WhiteQueen;
        bitBoardSet[PieceColor::White][PieceType::King] = Constants::InitialPositions::WhiteKing;

        bitBoardSet[PieceColor::Black][PieceType::Pawn] = Constants::InitialPositions::BlackPawns;
        bitBoardSet[PieceColor::Black][PieceType::Knight] = Constants::InitialPositions::BlackKnights;
        bitBoardSet[PieceColor::Black][PieceType::Bishop] = Constants::InitialPositions::BlackBishops;
        bitBoardSet[PieceColor::Black][PieceType::Rook] = Constants::InitialPositions::BlackRooks;
        bitBoardSet[PieceColor::Black][PieceType::Queen] = Constants::InitialPositions::BlackQueen;
        bitBoardSet[PieceColor::Black][PieceType::King] = Constants::InitialPositions::BlackKing;

        updateGenericBitBoards();
    }

    void Board::updateGenericBitBoards()
    {
        WhitePieces =
                bitBoardSet[PieceColor::White][PieceType::Pawn] | bitBoardSet[PieceColor::White][PieceType::Knight]
                | bitBoardSet[PieceColor::White][PieceType::Bishop] | bitBoardSet[PieceColor::White][PieceType::Rook]
                | bitBoardSet[PieceColor::White][PieceType::Queen] | bitBoardSet[PieceColor::White][PieceType::King];

        BlackPieces =
                bitBoardSet[PieceColor::Black][PieceType::Pawn] | bitBoardSet[PieceColor::Black][PieceType::Knight]
                | bitBoardSet[PieceColor::Black][PieceType::Bishop] | bitBoardSet[PieceColor::Black][PieceType::Rook]
                | bitBoardSet[PieceColor::Black][PieceType::Queen] | bitBoardSet[PieceColor::Black][PieceType::King];

        OccupiedSquares = WhitePieces | BlackPieces;
        EmptySquares = ~OccupiedSquares;
    }

    void Board::Display() const
    {
        Piece piece;

        for (int r = 7; r >= 0; r--)
        {
            std::cout << "   ------------------------\n";

            std::cout << " " << r+1 << " ";

            for (int c = 0; c <= 7; c++)
            {
                piece = pieceSet[Utils::Square::GetSquareIndex(c, r)];
                std::cout << '[';
                if (piece.Type != PieceType::None)
                {
                    std::cout << (piece.Color == PieceColor::White ? Console::Green : Console::Red);

                    std::cout << Utils::Piece::GetInitial(pieceSet[Utils::Square::GetSquareIndex(c, r)].Type);
                }
                else
                {
                    std::cout << Console::Red;
                    std::cout << ' ';
                }

                std::cout << Console::Reset;
                std::cout << ']';
            }
            std::cout << std::endl;
        }
        std::cout << "\n    A  B  C  D  E  F  G  H";
    }


}

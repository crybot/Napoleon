#include "board.h"
#include "utils.h"
#include "console.h"

namespace Napoleon
{
    Board::Board()
    {
        //MOVEDATABASE INITIALIZATION

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
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 8);
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 9);
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 10);
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 11);
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 12);
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 13);
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 14);
        AddPiece(Piece(PieceType::Pawn, PieceColor::White), 15);
        //
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 48);
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 49);
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 50);
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 51);
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 52);
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 53);
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 54);
        AddPiece(Piece(PieceType::Pawn, PieceColor::Black), 55);

        /*TORRI*/
        AddPiece(Piece(PieceType::Rook, PieceColor::White), 0);
        AddPiece(Piece(PieceType::Rook, PieceColor::White), 7);
        //
        AddPiece(Piece(PieceType::Rook, PieceColor::Black), 56);
        AddPiece(Piece(PieceType::Rook, PieceColor::Black), 63);

        /*CAVALLI*/
        AddPiece(Piece(PieceType::Knight, PieceColor::White), 1);
        AddPiece(Piece(PieceType::Knight, PieceColor::White), 6);
        //
        AddPiece(Piece(PieceType::Knight, PieceColor::Black), 57);
        AddPiece(Piece(PieceType::Knight, PieceColor::Black), 62);

        /*ALFIERI*/
        AddPiece(Piece(PieceType::Bishop, PieceColor::White), 2);
        AddPiece(Piece(PieceType::Bishop, PieceColor::White), 5);
        //
        AddPiece(Piece(PieceType::Bishop, PieceColor::Black), 58);
        AddPiece(Piece(PieceType::Bishop, PieceColor::Black), 61);

        /*RE*/
        AddPiece(Piece(PieceType::King, PieceColor::White), 4);
        //
        AddPiece(Piece(PieceType::King, PieceColor::Black), 60);

        /*REGINE*/
        AddPiece(Piece(PieceType::Queen, PieceColor::White), 3);
        //
        AddPiece(Piece(PieceType::Queen, PieceColor::Black), 59);
    }

    void Board::clearPieceSet()
    {
        for (int i=0; i<64; i++)
        {
            pieceSet[i] = Piece(PieceType::None, PieceColor::None);
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

    void Board::Display()
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
                    std::cout << ((piece.Color == PieceColor::White) ? Console::Green : Console::Red);

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

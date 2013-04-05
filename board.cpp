#include "board.h"
#include "utils.h"
#include "console.h"
#include "movedatabase.h"
#include "piece.h"
#include "fenstring.h"
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
        PieceSet[sq] = piece;
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
            PieceSet[i] = Piece(PieceColor::None, PieceType::None);
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
        KingSquare[PieceColor::White] = 4;
        KingSquare[PieceColor::Black] = 60;

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
                piece = PieceSet[Utils::Square::GetSquareIndex(c, r)];
                std::cout << '[';
                if (piece.Type != PieceType::None)
                {
                    std::cout << (piece.Color == PieceColor::White ? Console::Green : Console::Red);

                    std::cout << Utils::Piece::GetInitial(PieceSet[Utils::Square::GetSquareIndex(c, r)].Type);
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
        std::cout << "\n    A  B  C  D  E  F  G  H" << std::endl;

        std::cout << "Enpassant Square: " << Utils::Square::ToAlgebraic(EnPassantSquare) << std::endl;
        std::cout << "Side To Move: " << (SideToMove == PieceColor::White ? "White" : "Black") << std::endl;
    }

    void Board::LoadGame(const FenString& fenString)
    {
        initializeCastlingStatus(fenString);
        initializeSideToMove(fenString);
        initializePieceSet(fenString);
        initializeEnPassantSquare(fenString);
        initializeBitBoards(fenString);
    }

    void Board::initializeCastlingStatus(const FenString& fenString)
    {
        WhiteCanCastleOO = fenString.CanWhiteShortCastle;
        WhiteCanCastleOOO = fenString.CanWhiteLongCastle;
        BlackCanCastleOO = fenString.CanBlackShortCastle;
        BlackCanCastleOOO = fenString.CanBlackLongCastle;
    }

    void Board::initializeSideToMove(const FenString& fenString)
    {
        SideToMove = fenString.SideToMove;
    }

    void Board::initializePieceSet(const FenString& fenString)
    {
        for (int i=0; i<64; i++)
        {
            PieceSet[i] = fenString.PiecePlacement[i];
        }
    }

    void Board::initializeEnPassantSquare(const FenString& fenString)
    {
        EnPassantSquare = fenString.EnPassantSquare;
        enpSquares.push(EnPassantSquare);
    }

    void Board::initializeBitBoards(const FenString& fenString)
    {
        for (int i=PieceType::Pawn; i<PieceType::None; i++)
        {
            for (int l = PieceColor::White; l<PieceColor::None; l++)
            {
                bitBoardSet[l][i] = 0;
            }
        }

        for (int i = 0; i < 64; i++)
        {
            if (fenString.PiecePlacement[i].Type == PieceType::King)
                KingSquare[fenString.PiecePlacement[i].Color] = i;
            if (fenString.PiecePlacement[i].Color != PieceColor::None)
                bitBoardSet[fenString.PiecePlacement[i].Color][fenString.PiecePlacement[i].Type] |= Constants::Masks::SquareMask[i];
        }

        updateGenericBitBoards();
    }

    Move Board::ParseMove(std::string str)
    {
        Byte from = Utils::Square::Parse(str.substr(0, 2));
        Byte to = Utils::Square::Parse(str.substr(2));

        if (to == EnPassantSquare)
            return Move(from, to, PieceType::Pawn, PieceType::Pawn, PieceType::Pawn);

        return Move(from, to, PieceSet[from].Type, PieceSet[to].Type, PieceType::None);
    }

    void Board::MakeMove(const Move& move)
    {
        //ARRAY
        PieceSet[move.ToSquare] = PieceSet[move.FromSquare]; // muove il pezzo
        PieceSet[move.FromSquare] = Piece(PieceColor::None, PieceType::None); // svuota la casella di partenza

        //BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[move.FromSquare];
        BitBoard To = Constants::Masks::SquareMask[move.ToSquare];
        BitBoard FromTo = From | To;

        Byte enemy = Utils::Piece::GetOpposite(SideToMove);

        bitBoardSet[SideToMove][move.PieceMoved] ^= FromTo;
        if (SideToMove == PieceColor::White)
            WhitePieces ^= FromTo;
        else
            BlackPieces ^= FromTo;

        if (move.PieceMoved == PieceType::King)
            KingSquare[SideToMove] = move.ToSquare;


        if (move.IsEnPassant())
        {
            if (SideToMove == PieceColor::White)
            {
                BitBoard piece = Constants::Masks::SquareMask[EnPassantSquare - 8];
                bitBoardSet[enemy][move.PieceCaptured] ^= piece;
                BlackPieces ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;
                PieceSet[EnPassantSquare - 8] = Piece(PieceColor::None, PieceType::None);
            }
            else
            {
                BitBoard piece = Constants::Masks::SquareMask[EnPassantSquare + 8];
                bitBoardSet[enemy][move.PieceCaptured] ^= piece;
                WhitePieces ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;
                PieceSet[EnPassantSquare + 8] = Piece(PieceColor::None, PieceType::None);
            }
        }

        else if (move.IsCapture())
        {
            bitBoardSet[enemy][move.PieceCaptured] ^= To;

            //aggiorna i pezzi dell'avversario
            if (SideToMove == PieceColor::White)
                BlackPieces ^= To;
            else
                WhitePieces ^= To;

            OccupiedSquares ^= From;
            EmptySquares ^= From;
        }
        else
        {
            OccupiedSquares ^= FromTo;
            EmptySquares ^= FromTo;
        }

        enpSquares.push(EnPassantSquare);
        EnPassantSquare = Constants::Squares::Invalid;

        if (move.PieceMoved == PieceType::Pawn)
        {
            if (SideToMove == PieceColor::White)
            {
                if (Utils::Square::GetRankIndex(move.ToSquare) - Utils::Square::GetRankIndex(move.FromSquare) == 2)
                {
                    EnPassantSquare = move.ToSquare - 8;
                }
            }
            else
            {
                if (Utils::Square::GetRankIndex(move.FromSquare) - Utils::Square::GetRankIndex(move.ToSquare) == 2)
                {
                    EnPassantSquare = move.ToSquare + 8;
                }
            }
        }

        SideToMove = enemy;
    }

    void Board::UndoMove(const Move& move)
    {
        //ARRAY
        PieceSet[move.FromSquare] = PieceSet[move.ToSquare]; // muove il pezzo

        //BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[move.FromSquare];
        BitBoard To = Constants::Masks::SquareMask[move.ToSquare];
        BitBoard FromTo = From | To;

        Byte enemy = SideToMove;
        SideToMove = Utils::Piece::GetOpposite(SideToMove);

        // aggiorna la bitboard
        bitBoardSet[SideToMove][move.PieceMoved] ^= FromTo;
        if (SideToMove == PieceColor::White)
            WhitePieces ^= FromTo;
        else
            BlackPieces ^= FromTo;

        if (move.PieceMoved == PieceType::King)
            KingSquare[SideToMove] = move.FromSquare;

        EnPassantSquare = enpSquares.top();
        enpSquares.pop();

        if (move.IsEnPassant())
        {
            PieceSet[move.ToSquare] = Piece(PieceColor::None, PieceType::None); // svuota la casella di partenza perche` non c'erano pezzi prima

            if (SideToMove == PieceColor::White)
            {
                BitBoard enpSquare = Constants::Masks::SquareMask[EnPassantSquare];
                BitBoard piece = Constants::Masks::SquareMask[EnPassantSquare - 8];
                bitBoardSet[enemy][move.PieceCaptured] ^= piece;
                BlackPieces ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;
                PieceSet[EnPassantSquare - 8] = Piece(PieceColor::Black, PieceType::Pawn);
            }
            else
            {
                BitBoard enpSquare = Constants::Masks::SquareMask[EnPassantSquare];
                BitBoard piece = Constants::Masks::SquareMask[EnPassantSquare + 8];
                bitBoardSet[enemy][move.PieceCaptured] ^= piece;
                WhitePieces ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;
                PieceSet[EnPassantSquare + 8] = Piece(PieceColor::White, PieceType::Pawn);
            }
        }

        else if (move.IsCapture())
        {
            PieceSet[move.ToSquare] = Piece(enemy, move.PieceCaptured); // re-inserisce il pezzo catturato nella sua casella

            bitBoardSet[enemy][move.PieceCaptured] ^= To;

            //aggiorna i pezzi dell'avversario
            if (SideToMove == PieceColor::White)
                BlackPieces ^= To;
            else
                WhitePieces ^= To;

            OccupiedSquares ^= From;
            EmptySquares ^= From;
        }
        else
        {
            PieceSet[move.ToSquare] = Piece(PieceColor::None, PieceType::None); // svuota la casella di partenza perche` non c'erano pezzi prima
            OccupiedSquares ^= FromTo;
            EmptySquares ^= FromTo;
        }
    }

    bool Board::IsAttacked(BitBoard target, Byte side)
    {
        BitBoard slidingAttackers;
        BitBoard pawnAttacks;
        BitBoard allPieces = OccupiedSquares;
        Byte enemyColor = Utils::Piece::GetOpposite(side);
        int to;

        while (target != 0)
        {
            to = Utils::BitBoard::BitScanForwardReset(target);
            pawnAttacks = MoveDatabase::PawnAttacks[side][to];

            if ((GetPieceSet(enemyColor, PieceType::Pawn) & pawnAttacks) != 0) return true;
            if ((GetPieceSet(enemyColor, PieceType::Knight) & MoveDatabase::KnightAttacks[to]) != 0) return true;
            if ((GetPieceSet(enemyColor, PieceType::King) & MoveDatabase::KingAttacks[to]) != 0) return true;

            // file / rank attacks
            slidingAttackers = GetPieceSet(enemyColor, PieceType::Queen) | GetPieceSet(enemyColor, PieceType::Rook);

            if (slidingAttackers != 0)
            {
                if ((MoveDatabase::GetRankAttacks(allPieces, to) & slidingAttackers) != 0) return true;
                if ((MoveDatabase::GetFileAttacks(allPieces, to) & slidingAttackers) != 0) return true;
            }

            // diagonals
            slidingAttackers = GetPieceSet(enemyColor, PieceType::Queen) | GetPieceSet(enemyColor, PieceType::Bishop);

            if (slidingAttackers != 0)
            {
                if ((MoveDatabase::GetH1A8DiagonalAttacks(allPieces, to) & slidingAttackers) != 0) return true;
                if ((MoveDatabase::GetA1H8DiagonalAttacks(allPieces, to) & slidingAttackers) != 0) return true;
            }
        }
        return false;
    }






}

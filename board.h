#ifndef BOARD_H
#define BOARD_H
#include "constants.h"
#include "move.h"
#include "utils.h"
#include "movedatabase.h"
#include "compassrose.h"
#include <iostream>
#include <stack>

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

        Piece PieceSet[64];
        int KingSquare[2]; // color

        BitBoard WhitePieces;
        BitBoard BlackPieces;
        BitBoard OccupiedSquares;
        BitBoard EmptySquares;

        Board();

        void LoadGame(const FenString&);
        void Equip();

        void Display() const;
        void AddPiece(Piece, Square);
        BitBoard GetPlayerPieces() const;
        BitBoard GetEnemyPieces() const;
        BitBoard GetPieceSet(Byte, Byte) const;
        BitBoard GetPinnedPieces() const;
        BitBoard Attackers(int square, Byte color);
        BitBoard AttacksFrom(Byte type, int from, Byte color);

        void MakeMove(const Move&);
        void UndoMove(const Move&);
        bool IsMoveLegal(Move&, BitBoard);
        bool IsAttacked(BitBoard, Byte);

        Move ParseMove(std::string);

    private:
        std::stack<int> enpSquares;
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

    INLINE BitBoard Board::GetPlayerPieces() const
    {
        return SideToMove == PieceColor::White ? WhitePieces : BlackPieces;
    }

    INLINE BitBoard Board::GetEnemyPieces() const
    {
        return SideToMove == PieceColor::White ? BlackPieces : WhitePieces;
    }

    INLINE BitBoard Board::GetPieceSet(Byte pieceColor, Byte pieceType) const
    {
        return bitBoardSet[pieceColor][pieceType];
    }

    INLINE BitBoard Board::GetPinnedPieces() const
    {
        Byte enemy = Utils::Piece::GetOpposite(SideToMove);
        int kingSq = KingSquare[SideToMove];

        BitBoard playerPieces = GetPlayerPieces();
        BitBoard b;
        BitBoard pinned = 0;
        BitBoard pinners = ((bitBoardSet[enemy][PieceType::Rook] | bitBoardSet[enemy][PieceType::Queen] ) & MoveDatabase::PseudoRookAttacks[kingSq])
                | ((bitBoardSet[enemy][PieceType::Bishop] | bitBoardSet[enemy][PieceType::Queen]) & MoveDatabase::PseudoBishopAttacks[kingSq]);

        while (pinners)
        {
            int sq = Utils::BitBoard::BitScanForwardReset(pinners);
            b = MoveDatabase::ObstructedTable[sq][kingSq] & OccupiedSquares;

            if ((b != 0) && ((b & (b-1))==0) && ((b & playerPieces) != 0))
            {
                pinned |= b;
            }
        }
        return pinned;
    }

    INLINE bool Board::IsMoveLegal(Move& move, BitBoard pinned)
    {
        if (move.PieceMoved == PieceType::King)
        {
            return !Attackers(move.ToSquare, SideToMove);
        }

        if (move.IsEnPassant())
        {
            MakeMove(move);
            bool islegal = !IsAttacked(bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][PieceType::King], Utils::Piece::GetOpposite(SideToMove));
            UndoMove(move);

            return islegal;
        }

        return (pinned == 0) || ((pinned & Constants::Masks::SquareMask[move.FromSquare]) == 0)
                || MoveDatabase::AreSquareAligned(move.FromSquare, move.ToSquare,  KingSquare[SideToMove]);
    }

    INLINE BitBoard Board::Attackers(int square, Byte color)
    {
        Byte opp = Utils::Piece::GetOpposite(color);
        BitBoard bishopAttacks = MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, square)
                | MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, square);
        BitBoard rookAttacks =MoveDatabase::GetFileAttacks(OccupiedSquares, square)
                | MoveDatabase::GetRankAttacks(OccupiedSquares, square);

        return (MoveDatabase::PawnAttacks[color][square] & bitBoardSet[opp][PieceType::Pawn])
                | (MoveDatabase::KnightAttacks[square] & bitBoardSet[opp][PieceType::Knight])
                | (bishopAttacks  & (bitBoardSet[opp][PieceType::Bishop] | bitBoardSet[opp][PieceType::Queen]))
                | (rookAttacks   & (bitBoardSet[opp][PieceType::Rook] | bitBoardSet[opp][PieceType::Queen]));
    }

}

#endif // BOARD_H

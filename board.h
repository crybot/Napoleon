#ifndef BOARD_H
#define BOARD_H
#include "constants.h"
#include "move.h"
#include "utils.h"
#include "movedatabase.h"
#include "transpositiontable.h"
#include "zobrist.h"
#include "uci.h"
#include <cassert>
#include <iostream>
#include <string>

namespace Napoleon
{
    class MoveList;
    class FenString;
    class Board
    {
    public:
        bool AllowNullMove;

        bool IsCheck;
        int HalfMoveClock;
        int EnPassantSquare;
        int CurrentPly;
        Byte CastlingStatus;
        Byte SideToMove;
        unsigned long Nps;
        unsigned long FirstMoveCutoff;
        unsigned long TotalCutoffs;

        ZobristKey hash[Constants::MaxPly]; // debugging

        int KingSquare[2]; // color
        int PSTValue[2]; // color
        int Material[2]; // color
//        int NumOfPieces[2][6]; // color, type
        Piece PieceSet[64]; // square

        BitBoard bitBoardSet[2][6]; // color, type
        BitBoard Pieces[2]; // color

        BitBoard OccupiedSquares;
        BitBoard EmptySquares;

        ZobristKey zobrist;
        TranspositionTable Table;

        Board();

        void LoadGame(const FenString&);
        void Equip();

        void Display() const;
        void AddPiece(Piece, Square);
        BitBoard GetPlayerPieces() const;
        BitBoard GetEnemyPieces() const;
        BitBoard GetPieceSet(Byte, Byte) const;
        BitBoard GetPinnedPieces() const;
        BitBoard KingAttackers(int square, Byte color) const;

        void MakeMove(Move);
        void UndoMove(Move);
        void MakeNullMove();
        void UndoNullMove();

        bool IsCapture(Move) const;
        bool IsMoveLegal(Move, BitBoard);
        bool IsAttacked(BitBoard, Byte) const;
        bool IsPromotingPawn() const;
        bool IsOnSquare(Byte, Byte, Square) const;

        Move ParseMove(std::string) const;

        std::string GetFen() const;

    private:

        int enpSquares[Constants::MaxPly];
        int halfMoveClock[Constants::MaxPly];
        Byte castlingStatus[Constants::MaxPly];
        Byte capturedPiece[Constants::MaxPly];

        void clearPieceSet();
        void updateGenericBitBoards();
        void initializePieceSet();
        void initializeCastlingStatus();
        void initializeSideToMove();
        void initializeEnPassantSquare();
        void initializeHalfMoveClock();
        void initializeBitBoards();
        void initializeBitBoards(const FenString&);
        void initializeSideToMove(const FenString&);
        void initializeCastlingStatus(const FenString&);
        void initializeEnPassantSquare(const FenString&);
        void initializeHalfMoveClock(const FenString&);
        void initializePieceSet(const FenString&);
        void makeCastle(int, int);
        void undoCastle(int, int);
    };

    inline BitBoard Board::GetPlayerPieces() const
    {
        return Pieces[SideToMove];
    }

    inline BitBoard Board::GetEnemyPieces() const
    {
        return Pieces[Utils::Piece::GetOpposite(SideToMove)];
    }

    inline BitBoard Board::GetPieceSet(Byte pieceColor, Byte pieceType) const
    {
        return bitBoardSet[pieceColor][pieceType];
    }

    inline bool Board::IsOnSquare(Byte color, Byte type, Square sq) const
    {
        return (bitBoardSet[color][type] & Constants::Masks::SquareMask[sq]);
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

    INLINE bool Board::IsMoveLegal(Move move, BitBoard pinned)
    {
        if (PieceSet[move.FromSquare()].Type == PieceType::King)
        {
            return !IsAttacked(Constants::Masks::SquareMask[move.ToSquare()], SideToMove);
        }

        if (move.IsEnPassant())
        {
            MakeMove(move);
            bool islegal = !IsAttacked(bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][PieceType::King], Utils::Piece::GetOpposite(SideToMove));
            UndoMove(move);
            return islegal;
        }

        return (pinned == 0) || ((pinned & Constants::Masks::SquareMask[move.FromSquare()]) == 0)
                || MoveDatabase::AreSquareAligned(move.FromSquare(), move.ToSquare(),  KingSquare[SideToMove]);
    }

    INLINE BitBoard Board::KingAttackers(int square, Byte color) const
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

    inline void Board::MakeNullMove()
    {
        hash[CurrentPly] = zobrist;
        enpSquares[CurrentPly] = EnPassantSquare;
        SideToMove = Utils::Piece::GetOpposite(SideToMove);
        EnPassantSquare = Constants::Squares::Invalid;

        zobrist ^= Zobrist::Color;

        if (EnPassantSquare != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(EnPassantSquare)];

        if (enpSquares[CurrentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquares[CurrentPly])];

        AllowNullMove = false;
        CurrentPly++;
    }

    inline void Board::UndoNullMove()
    {
        CurrentPly--;
        SideToMove = Utils::Piece::GetOpposite(SideToMove);
        EnPassantSquare = enpSquares[CurrentPly];

        zobrist ^= Zobrist::Color;

        if (enpSquares[CurrentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquares[CurrentPly])];

        AllowNullMove = true;
    }

    inline bool Board::IsPromotingPawn() const
    {
        const BitBoard rank = (SideToMove == PieceColor::White ? Constants::Ranks::Seven : Constants::Ranks::Two);
        return (bitBoardSet[SideToMove][PieceType::Pawn] & rank);
    }

    inline bool Board::IsCapture(Move move) const
    {
        return (PieceSet[move.ToSquare()].Type != PieceType::None);
    }
}

#endif // BOARD_H

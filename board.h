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
        unsigned long FirstMoveCutoff;
        unsigned long TotalCutoffs;

        //        int NumOfPieces[2][6]; // color, type
        ZobristKey hash[Constants::MaxPly]; // debugging

        Piece PieceSet[64]; // square
        BitBoard Pieces[2]; // color

        BitBoard OccupiedSquares;
        BitBoard EmptySquares;

        ZobristKey zobrist;
        TranspositionTable Table;

        Board();

        void LoadGame(std::string = Constants::StartPosition);

        void Display() const;
        void AddPiece(Piece, Square);
        BitBoard GetPlayerPieces() const;
        BitBoard GetEnemyPieces() const;
        BitBoard GetPieces(Color, Type) const;
        BitBoard GetPieces(Color) const;
        BitBoard GetPinnedPieces() const;
        BitBoard KingAttackers(Square, Color) const;

        void MakeMove(Move);
        void UndoMove(Move);
        void MakeNullMove();
        void UndoNullMove();

        void SetCheckState(bool);

        bool IsCapture(Move) const;
        bool IsMoveLegal(Move, BitBoard);
        bool IsAttacked(BitBoard, Color) const;
        bool IsPromotingPawn() const;
        bool IsOnSquare(Color, Type, Square) const;
        bool IsRepetition() const;

        Square KingSquare(Color) const;

        Color SideToMove() const;
        Byte CastlingStatus() const;
        Square EnPassantSquare() const;
        int HalfMoveClock() const;
        int CurrentPly() const;
        bool AllowNullMove() const;
        bool IsCheck() const;

        int PstValue(Color) const;
        int Material(Color) const;
        int PawnsOnFile(Color, File) const;

        bool PosIsOk() const;

        Move ParseMove(std::string) const;
        std::string GetFen() const;

    private:
        BitBoard bitBoardSet[2][6]; // color, type
        Byte castlingStatusHistory[Constants::MaxPly];
        Type capturedPiece[Constants::MaxPly];
        Square enpSquares[Constants::MaxPly];
        Square kingSquare[2]; // color

        Color sideToMove;
        Byte castlingStatus;
        Square enPassantSquare;

        int halfMoveClock;
        int currentPly;
        bool allowNullMove;
        bool isCheck;

        int halfMoveClockHistory[Constants::MaxPly];
        int pawnsOnFile[2][8]; // color, file
        int pstValue[2]; // color
        int material[2]; // color

        void clearPieceSet();
        void updateGenericBitBoards();
        void initializeBitBoards(const FenString&);
        void initializesideToMove(const FenString&);
        void initializeCastlingStatus(const FenString&);
        void initializeEnPassantSquare(const FenString&);
        void initializeHalfMoveClock(const FenString&);
        void initializePieceSet(const FenString&);
        void makeCastle(Square, Square);
        void undoCastle(Square, Square);
    };

    inline Byte Board::CastlingStatus() const
    {
        return castlingStatus;
    }

    INLINE Color Board::SideToMove() const
    {
        return sideToMove;
    }

    inline Square Board::EnPassantSquare() const
    {
        return enPassantSquare;
    }

    inline int Board::HalfMoveClock() const
    {
        return halfMoveClock;
    }

    inline int Board::CurrentPly() const
    {
        return currentPly;
    }

    inline bool Board::AllowNullMove() const
    {
        return allowNullMove;
    }

    inline bool Board::IsCheck() const
    {
        return isCheck;
    }

    inline void Board::SetCheckState(bool isCheck)
    {
        this->isCheck = isCheck;
    }

    INLINE BitBoard Board::GetPinnedPieces() const
    {
        Byte enemy = Utils::Piece::GetOpposite(sideToMove);
        int kingSq = kingSquare[sideToMove];

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
            return !IsAttacked(Constants::Masks::SquareMask[move.ToSquare()], sideToMove);
        }

        if (move.IsEnPassant())
        {
            MakeMove(move);
            bool islegal = !IsAttacked(bitBoardSet[Utils::Piece::GetOpposite(sideToMove)][PieceType::King], Utils::Piece::GetOpposite(sideToMove));
            UndoMove(move);
            return islegal;
        }

        return (pinned == 0) || ((pinned & Constants::Masks::SquareMask[move.FromSquare()]) == 0)
                || MoveDatabase::AreSquareAligned(move.FromSquare(), move.ToSquare(),  kingSquare[sideToMove]);
    }

    INLINE BitBoard Board::KingAttackers(Square square, Byte color) const
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
        hash[currentPly] = zobrist;
        enpSquares[currentPly] = enPassantSquare;
        sideToMove = Utils::Piece::GetOpposite(sideToMove);
        enPassantSquare = Constants::Squares::Invalid;

        zobrist ^= Zobrist::Color;

        if (enPassantSquare != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];

        if (enpSquares[currentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquares[currentPly])];

        allowNullMove = false;
        currentPly++;
    }

    inline void Board::UndoNullMove()
    {
        currentPly--;
        sideToMove = Utils::Piece::GetOpposite(sideToMove);
        enPassantSquare = enpSquares[currentPly];

        zobrist ^= Zobrist::Color;

        if (enpSquares[currentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquares[currentPly])];

        allowNullMove = true;
    }

    inline BitBoard Board::GetPlayerPieces() const
    {
        return Pieces[sideToMove];
    }

    inline BitBoard Board::GetEnemyPieces() const
    {
        return Pieces[Utils::Piece::GetOpposite(sideToMove)];
    }

    inline BitBoard Board::GetPieces(Color color, Type type) const
    {
        return bitBoardSet[color][type];
    }

    inline BitBoard Board::GetPieces(Color color) const
    {
        return Pieces[color];
    }

    inline bool Board::IsPromotingPawn() const
    {
        const BitBoard rank = (sideToMove == PieceColor::White ? Constants::Ranks::Seven : Constants::Ranks::Two);
        return (bitBoardSet[sideToMove][PieceType::Pawn] & rank);
    }

    inline bool Board::IsCapture(Move move) const
    {
        return (PieceSet[move.ToSquare()].Type != PieceType::None);
    }

    inline bool Board::IsOnSquare(Color color, Type type, Square sq) const
    {
        return (bitBoardSet[color][type] & Constants::Masks::SquareMask[sq]);
    }

    inline int Board::PawnsOnFile(Color color, File file) const
    {
        return pawnsOnFile[color][file];
    }

    inline Square Board::KingSquare(Color color) const
    {
        return kingSquare[color];
    }

    inline int Board::PstValue(Color color) const
    {
        return pstValue[color];
    }

    inline int Board::Material(Color color) const
    {
        return material[color];
    }

    inline bool Board::IsRepetition() const
    {
        for(int i=0; i<currentPly; i++)
        {
            if (hash[i] == zobrist)
                return true;
        }
        return false;
    }

    // used for debug
    inline bool Board::PosIsOk() const
    {
        BitBoard playerPieces = 0;
        BitBoard enemyPieces = 0;
        Color enemy = Utils::Piece::GetOpposite(sideToMove);

        for (int i=0; i<PieceType::None; i++)
        {
            playerPieces |= bitBoardSet[sideToMove][i];
            enemyPieces |= bitBoardSet[enemy][i];
        }

        if (sideToMove != PieceColor::White && sideToMove != PieceColor::Black)
            return false;

        if (GetPlayerPieces() & GetEnemyPieces())
            return false;

        if (playerPieces & enemyPieces)
            return false;

        if (playerPieces != GetPlayerPieces())
            return false;

        if (enemyPieces != GetEnemyPieces())
            return false;

        if ((playerPieces | enemyPieces) != OccupiedSquares)
            return false;

        if (PieceSet[kingSquare[PieceColor::White]].Color != PieceColor::White)
            return false;

        if (PieceSet[kingSquare[PieceColor::Black]].Color != PieceColor::Black)
            return false;

        return true;
    }
}

#endif // BOARD_H

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

        Piece pieceSet[64];

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

        void MakeMove(const Move&);
        void UndoMove(const Move&);
        bool IsMoveLegal(Move&, BitBoard);
        bool IsAttacked(BitBoard, Byte);

    private:
        std::stack<int> enpSquares;
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

    INLINE void Board::MakeMove(const Move& move)
    {
        //ARRAY
        pieceSet[move.ToSquare] = pieceSet[move.FromSquare]; // muove il pezzo
        pieceSet[move.FromSquare] = Piece(PieceColor::None, PieceType::None); // svuota la casella di partenza

        //BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[move.FromSquare];
        BitBoard To = Constants::Masks::SquareMask[move.ToSquare];
        BitBoard FromTo = From | To;


        bitBoardSet[SideToMove][move.PieceMoved] ^= FromTo;
        if (SideToMove == PieceColor::White)
            WhitePieces ^= FromTo;
        else
            BlackPieces ^= FromTo;

        if (move.PieceMoved == PieceType::King)
            kingSquare[SideToMove] = move.ToSquare;


        if (move.IsEnPassant())
        {
            if (SideToMove == PieceColor::White)
            {
                BitBoard enpSquare = CompassRose::OneStepSouth(Constants::Masks::SquareMask[move.ToSquare]);
                bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][move.PieceCaptured] ^= enpSquare;
                BlackPieces ^= enpSquare;
                OccupiedSquares ^= FromTo ^ enpSquare;
                EmptySquares ^= FromTo ^ enpSquare;
                pieceSet[EnPassantSquare] = Piece(PieceColor::None, PieceType::None);
            }
            else
            {
                BitBoard enpSquare = CompassRose::OneStepNorth(Constants::Masks::SquareMask[move.ToSquare]);
                bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][move.PieceCaptured] ^= enpSquare;
                WhitePieces ^= enpSquare;
                OccupiedSquares ^= FromTo ^ enpSquare;
                EmptySquares ^= FromTo ^ enpSquare;
                pieceSet[EnPassantSquare] = Piece(PieceColor::None, PieceType::None);
            }
        }

        else if (move.IsCapture())
        {
            bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][move.PieceCaptured] ^= To;

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

        SideToMove = Utils::Piece::GetOpposite(SideToMove);
    }

    INLINE void Board::UndoMove(const Move& move)
    {
        //ARRAY
        pieceSet[move.FromSquare] = pieceSet[move.ToSquare]; // muove il pezzo

        //BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[move.FromSquare];
        BitBoard To = Constants::Masks::SquareMask[move.ToSquare];
        BitBoard FromTo = From | To;

        SideToMove = Utils::Piece::GetOpposite(SideToMove);

        // aggiorna la bitboard
        bitBoardSet[SideToMove][move.PieceMoved] ^= FromTo;
        if (SideToMove == PieceColor::White)
            WhitePieces ^= FromTo;
        else
            BlackPieces ^= FromTo;

        if (move.PieceMoved == PieceType::King)
            kingSquare[SideToMove] = move.FromSquare;

        EnPassantSquare = enpSquares.top();
        enpSquares.pop();

        if (move.IsEnPassant())
        {
            pieceSet[move.ToSquare] = Piece(PieceColor::None, PieceType::None); // svuota la casella di partenza perche` non c'erano pezzi prima

            if (SideToMove == PieceColor::White)
            {
                BitBoard enpSquare = CompassRose::OneStepSouth(Constants::Masks::SquareMask[move.ToSquare]);
                bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][move.PieceCaptured] ^= enpSquare;
                BlackPieces ^= enpSquare;
                OccupiedSquares ^= FromTo ^ enpSquare;
                EmptySquares ^= FromTo ^ enpSquare;
                pieceSet[EnPassantSquare] = Piece(PieceColor::Black, PieceType::Pawn);


            }
            else
            {
                BitBoard enpSquare = CompassRose::OneStepNorth(Constants::Masks::SquareMask[move.ToSquare]);
                bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][move.PieceCaptured] ^= enpSquare;
                WhitePieces ^= enpSquare;
                OccupiedSquares ^= FromTo ^ enpSquare;
                EmptySquares ^= FromTo ^ enpSquare;
                pieceSet[EnPassantSquare] = Piece(PieceColor::Black, PieceType::Pawn);
            }


        }

        else if (move.IsCapture())
        {
            pieceSet[move.ToSquare] = Piece(SideToMove, move.PieceCaptured); // re-inserisce il pezzo catturato nella sua casella

            bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][move.PieceCaptured] ^= To;

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
            pieceSet[move.ToSquare] = Piece(PieceColor::None, PieceType::None); // svuota la casella di partenza perche` non c'erano pezzi prima
            OccupiedSquares ^= FromTo;
            EmptySquares ^= FromTo;
        }

        //        if (move.PieceMoved == PieceType::Pawn)
        //        {
        //            if (SideToMove == PieceColor::White)
        //            {
        //                if (Utils::Square::GetRankIndex(move.ToSquare) - Utils::Square::GetRankIndex(move.FromSquare) == 2)
        //                {
        //                    EnPassantSquare = enpSquares.top();
        //                    enpSquares.pop();
        //                }
        //            }
        //            else
        //            {
        //                if (Utils::Square::GetRankIndex(move.FromSquare) - Utils::Square::GetRankIndex(move.ToSquare) == 2)
        //                {
        //                    EnPassantSquare = enpSquares.top();
        //                    enpSquares.pop();
        //                }
        //            }
        //        }
        //        else
        //        {
        //            EnPassantSquare = enpSquares.top();
        //            enpSquares.pop();
        //        }
    }

    INLINE BitBoard Board::GetPinnedPieces() const
    {
        Byte enemy = Utils::Piece::GetOpposite(SideToMove);
        int kingSq = kingSquare[SideToMove];

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
            return !IsAttacked(Constants::Masks::SquareMask[move.ToSquare], SideToMove);

        if (IsAttacked(bitBoardSet[SideToMove][PieceType::King], SideToMove))
        {
            MakeMove(move);
            bool islegal = !IsAttacked(bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][PieceType::King], Utils::Piece::GetOpposite(SideToMove));
            UndoMove(move);

            return islegal;
        }

        if (move.IsEnPassant())
        {
            MakeMove(move);
            bool islegal = !IsAttacked(bitBoardSet[Utils::Piece::GetOpposite(SideToMove)][PieceType::King], Utils::Piece::GetOpposite(SideToMove));
            UndoMove(move);

            return islegal;
        }
        return (pinned == 0) || ((pinned & Constants::Masks::SquareMask[move.FromSquare]) == 0)
                || MoveDatabase::AreSquareAligned(move.FromSquare, move.ToSquare, kingSquare[SideToMove]);
    }


    INLINE bool Board::IsAttacked(BitBoard target, Byte side)
    {
        BitBoard slidingAttackers;
        BitBoard pawnAttacks;
        BitBoard allPieces = OccupiedSquares;
        Byte enemyColor = Utils::Piece::GetOpposite(side);
        int to;

        while (target != 0)
        {
            to = Utils::BitBoard::BitScanForwardReset(target);
            pawnAttacks = side == PieceColor::White ? MoveDatabase::WhitePawnAttacks[to] : MoveDatabase::BlackPawnAttacks[to];

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

#endif // BOARD_H

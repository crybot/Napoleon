#ifndef BOARD_H
#define BOARD_H
#include "constants.h"
#include "move.h"
#include "utils.h"
#include "movedatabase.h"
#include <cassert>
#include <iostream>

namespace Napoleon
{
    class MoveList;
    class FenString;
    class Board
    {
    public:
        int EnPassantSquare;
        Byte CastlingStatus;
        Byte SideToMove;

        int KingSquare[2]; // color
        int NumOfPieces[2][6] = { {0} }; // color, type
        Piece PieceSet[64]; // square

        BitBoard bitBoardSet[2][6] = { { Constants::Empty } }; // color, type
        BitBoard Pieces[2]; // color

        BitBoard OccupiedSquares;
        BitBoard EmptySquares;

        ZobristKey zobrist;

        Board();

        void LoadGame(const FenString&);
        void Equip();

        void Display() const;
        void AddPiece(Piece, Square);
        BitBoard GetPlayerPieces() const;
        BitBoard GetEnemyPieces() const;
        BitBoard GetPieceSet(Byte, Byte) const;
        BitBoard GetPinnedPieces() const;
        BitBoard KingAttackers(int square, Byte color);

        void MakeMove(Move);
        void UndoMove(Move);
        bool IsMoveLegal(Move&, BitBoard);
        bool IsAttacked(BitBoard, Byte);

        int Evaluate();

        Move ParseMove(std::string, MoveList);

    private:
        int ply;
        int enpSquares[Constants::MaxPly];
        Byte castlingStatus[Constants::MaxPly];


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
        void makeCastle(int, int);
        void undoCastle(int, int);
        template <Byte piece>
        int evaluateMobility(BitBoard);

    };

    INLINE BitBoard Board::GetPlayerPieces() const
    {
        return Pieces[SideToMove];
    }

    INLINE BitBoard Board::GetEnemyPieces() const
    {
        return Pieces[Utils::Piece::GetOpposite(SideToMove)];
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
        if (PieceSet[move.FromSquare].Type == PieceType::King)
        {
            return !IsAttacked(Constants::Masks::SquareMask[move.ToSquare], SideToMove);
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

    INLINE BitBoard Board::KingAttackers(int square, Byte color)
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

    INLINE int Board::Evaluate()
    {
        using namespace Utils::BitBoard;
        float material = 2000*(NumOfPieces[PieceColor::White][PieceType::King] - NumOfPieces[PieceColor::Black][PieceType::King])
                +   900*(NumOfPieces[PieceColor::White][PieceType::Queen] - NumOfPieces[PieceColor::Black][PieceType::Queen])
                +   500*(NumOfPieces[PieceColor::White][PieceType::Rook] - NumOfPieces[PieceColor::Black][PieceType::Rook])
                +   330*(NumOfPieces[PieceColor::White][PieceType::Bishop] - NumOfPieces[PieceColor::Black][PieceType::Bishop])
                +   320*(NumOfPieces[PieceColor::White][PieceType::Knight] - NumOfPieces[PieceColor::Black][PieceType::Knight])
                +   100*(NumOfPieces[PieceColor::White][PieceType::Pawn] - NumOfPieces[PieceColor::Black][PieceType::Pawn]);


        int wM = 0;
        int bM = 0;

        //        while(whiteBishops)
        //        {
        //            square = BitScanForwardReset(whiteBishops);

        //            w |= MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, square);
        //        }

        //        while(whiteQueens)
        //        {
        //            square = BitScanForwardReset(whiteQueens);

        //            w |= MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetFileAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetRankAttacks(OccupiedSquares, square);
        //        }

        //        while(blackQueens)
        //        {
        //            square = BitScanForwardReset(blackQueens);

        //            b |= MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetFileAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetRankAttacks(OccupiedSquares, square);
        //        }

        //        while(blackBishops)
        //        {
        //            square = BitScanForwardReset(blackBishops);

        //            b |= MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, square)
        //                    | MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, square);
        //        }

        wM += evaluateMobility<PieceType::Bishop>(bitBoardSet[PieceColor::White][PieceType::Bishop]);
        bM += evaluateMobility<PieceType::Bishop>(bitBoardSet[PieceColor::Black][PieceType::Bishop]);

        wM += evaluateMobility<PieceType::Queen>(bitBoardSet[PieceColor::White][PieceType::Queen]);
        bM += evaluateMobility<PieceType::Queen>(bitBoardSet[PieceColor::Black][PieceType::Queen]);



        return (material + (wM - bM)) * (1-(SideToMove*2));
    }

    template <Byte piece>
    INLINE int Board::evaluateMobility(BitBoard pieces)
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

                b |= MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, square)
                        | MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, square);
            }

            return PopCount(b);

        case PieceType::Queen:
            while(pieces)
            {
                square = BitScanForwardReset(pieces);

                b |= MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, square)
                        | MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, square)
                        | MoveDatabase::GetFileAttacks(OccupiedSquares, square)
                        | MoveDatabase::GetRankAttacks(OccupiedSquares, square);
            }
            return PopCount(b);

        default:
            break;
        }

        return 0;
    }
}

#endif // BOARD_H

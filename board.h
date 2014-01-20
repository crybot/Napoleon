#ifndef BOARD_H
#define BOARD_H
#include "constants.h"
#include "move.h"
#include "utils.h"
#include "movedatabase.h"
#include "transpositiontable.h"
#include "zobrist.h"
#include "uci.h"
#include "pawn.h"
#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>

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

        BitBoard OccupiedSquares;
        BitBoard EmptySquares;

        ZobristKey zobrist;
        TranspositionTable Table;

        Board();

        void LoadGame(std::string = Constants::StartPosition);

        void Display() const;
        void AddPiece(Piece, Square);
        BitBoard PlayerPieces() const;
        BitBoard EnemyPieces() const;
        BitBoard Pieces(Color, Type) const;
        BitBoard Pieces(Color) const;
        BitBoard PinnedPieces() const;
        BitBoard KingAttackers(Square, Color) const;
        BitBoard AttacksTo(Square, Color, BitBoard) const;
        BitBoard MovesTo(Square, Color, BitBoard) const;
        std::pair<BitBoard, Type> LeastValuableAttacker(Color, BitBoard) const;

        int See(Move) const;

        Piece PieceOnSquare(Square) const;

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
        int MaterialBalance(Color) const;
        int PawnsOnFile(Color, File) const;

        bool PosIsOk() const;

        std::string GetFen() const;
        Move ParseMove(std::string) const;

    private:

        // used to restore previous board state after MakeMove()
        Byte castlingStatusHistory[Constants::MaxPly];
        Type capturedPieceHistory[Constants::MaxPly];
        Square enpSquaresHistory[Constants::MaxPly];
        ZobristKey hashHistory[Constants::MaxPly]; // only useful for debugging (check if previous computed hash != actual hash)
        int halfMoveClockHistory[Constants::MaxPly];

        BitBoard bitBoardSet[2][6]; // color, type
        Square kingSquare[2]; // color

        Piece pieceSet[64]; // square
        BitBoard pieces[2]; // color

        Color sideToMove;
        Byte castlingStatus;
        Square enPassantSquare;

        int halfMoveClock;
        int currentPly;
        bool allowNullMove;
        bool isCheck;

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

    INLINE BitBoard Board::PinnedPieces() const
    {
        Byte enemy = Utils::Piece::GetOpposite(sideToMove);
        int kingSq = kingSquare[sideToMove];

        BitBoard playerPieces = PlayerPieces();
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
        if (pieceSet[move.FromSquare()].Type == PieceType::King)
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

    INLINE BitBoard Board::AttacksTo(Square square, Color color, BitBoard occ) const
    {
        Byte opp = Utils::Piece::GetOpposite(color);
        BitBoard bishopAttacks = MoveDatabase::GetA1H8DiagonalAttacks(occ, square) | MoveDatabase::GetH1A8DiagonalAttacks(occ, square);
        BitBoard rookAttacks = MoveDatabase::GetFileAttacks(occ, square)| MoveDatabase::GetRankAttacks(occ, square);

        return
                (MoveDatabase::KingAttacks[square] & bitBoardSet[color][PieceType::King])
                | (MoveDatabase::PawnAttacks[opp][square] & bitBoardSet[color][PieceType::Pawn])
                | (MoveDatabase::KnightAttacks[square] & bitBoardSet[color][PieceType::Knight])
                | (bishopAttacks  & (bitBoardSet[color][PieceType::Bishop] | bitBoardSet[color][PieceType::Queen]))
                | (rookAttacks   & (bitBoardSet[color][PieceType::Rook] | bitBoardSet[color][PieceType::Queen]));
    }

    INLINE BitBoard Board::MovesTo(Square square, Color color, BitBoard occ) const
    {
        BitBoard bishopAttacks = MoveDatabase::GetA1H8DiagonalAttacks(occ, square) | MoveDatabase::GetH1A8DiagonalAttacks(occ, square);
        BitBoard rookAttacks = MoveDatabase::GetFileAttacks(occ, square)| MoveDatabase::GetRankAttacks(occ, square);

        Square pawnSquare;
        BitBoard pawn = 0;
        BitBoard pawnMove = 0;
        Color enemy = Utils::Piece::GetOpposite(color);

        if (color == PieceColor::White)
        {
            if (square >= 16)
            {
                pawnSquare = square - 16;
                pawn = Constants::Masks::SquareMask[pawnSquare] & bitBoardSet[color][PieceType::Pawn];
                pawnMove = Pawn::GetQuietTargets(color, pawn, EmptySquares) & Constants::Masks::SquareMask[square];
            }
            if (square >= 8)
            {
                pawnSquare = square - 8;
                pawn |= Constants::Masks::SquareMask[pawnSquare] & bitBoardSet[color][PieceType::Pawn];
                pawnMove |= Pawn::GetQuietTargets(color, pawn, EmptySquares) & Constants::Masks::SquareMask[square];
            }
        }
        else
        {
            if (square <= (63 - 16))
            {
                pawnSquare = square + 16;
                pawn = Constants::Masks::SquareMask[pawnSquare] & bitBoardSet[color][PieceType::Pawn];
                pawnMove = Pawn::GetQuietTargets(color, pawn, EmptySquares) & Constants::Masks::SquareMask[square];
            }
            if (square <= (63 - 8))
            {
                pawnSquare = square + 8;
                pawn |= Constants::Masks::SquareMask[pawnSquare] & bitBoardSet[color][PieceType::Pawn];
                pawnMove |= Pawn::GetQuietTargets(color, pawn, EmptySquares) & Constants::Masks::SquareMask[square];
            }
        }

        return
                (MoveDatabase::KingAttacks[square] & bitBoardSet[color][PieceType::King])
                |

                ( Constants::Masks::SquareMask[square] & Pieces(enemy) ?
                      (MoveDatabase::PawnAttacks[enemy][square] & bitBoardSet[color][PieceType::Pawn])  : 0)

            | (pawn)

                    | (MoveDatabase::KnightAttacks[square] & bitBoardSet[color][PieceType::Knight])
                    | (bishopAttacks  & (bitBoardSet[color][PieceType::Bishop] | bitBoardSet[color][PieceType::Queen]))
                    | (rookAttacks   & (bitBoardSet[color][PieceType::Rook] | bitBoardSet[color][PieceType::Queen]));

        }


        inline void Board::MakeNullMove()
        {
        hashHistory[currentPly] = zobrist;
        enpSquaresHistory[currentPly] = enPassantSquare;
        sideToMove = Utils::Piece::GetOpposite(sideToMove);
        enPassantSquare = Constants::Squares::Invalid;

        zobrist ^= Zobrist::Color;

        if (enPassantSquare != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];

        if (enpSquaresHistory[currentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquaresHistory[currentPly])];

        allowNullMove = false;
        currentPly++;
    }

    inline void Board::UndoNullMove()
    {
        currentPly--;
        sideToMove = Utils::Piece::GetOpposite(sideToMove);
        enPassantSquare = enpSquaresHistory[currentPly];

        zobrist ^= Zobrist::Color;

        if (enpSquaresHistory[currentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquaresHistory[currentPly])];

        allowNullMove = true;
    }

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

    inline BitBoard Board::PlayerPieces() const
    {
        return pieces[sideToMove];
    }

    inline BitBoard Board::EnemyPieces() const
    {
        return pieces[Utils::Piece::GetOpposite(sideToMove)];
    }

    inline BitBoard Board::Pieces(Color color, Type type) const
    {
        return bitBoardSet[color][type];
    }

    inline BitBoard Board::Pieces(Color color) const
    {
        return pieces[color];
    }

    inline Piece Board::PieceOnSquare(Square square) const
    {
        return pieceSet[square];
    }

    inline bool Board::IsPromotingPawn() const
    {
        const BitBoard rank = (sideToMove == PieceColor::White ? Constants::Ranks::Seven : Constants::Ranks::Two);
        return (bitBoardSet[sideToMove][PieceType::Pawn] & rank);
    }

    inline bool Board::IsCapture(Move move) const
    {
        return (pieceSet[move.ToSquare()].Type != PieceType::None || move.IsEnPassant());
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

    inline int Board::MaterialBalance(Color color) const
    {
        return material[color] - material[Utils::Piece::GetOpposite(color)];
    }

    inline std::pair<BitBoard, Type> Board::LeastValuableAttacker(Color color, BitBoard attackers) const
    {
        for(Type type = PieceType::Pawn; type < PieceType::None; type++)
        {
            BitBoard set = Pieces(color, type) & attackers;
            if (set)
                return std::make_pair(set & -set, type);
        }
        return std::make_pair(Constants::Empty, PieceType::None);
    }

    inline int Board::See(Move move) const
    {
        using namespace Constants::Masks;
        using namespace Constants::Piece;
        using namespace Utils::Piece;

        //assert(IsCapture(move));

        Square to = move.ToSquare();
        Square from = move.FromSquare();
        Piece captured = PieceOnSquare(to);
        Type attackingPiece = PieceOnSquare(from).Type;

        //if (PieceValue[PieceOnSquare(from).Type] <= PieceValue[PieceOnSquare(to).Type])
        //	return 1;

        int gain[100];
        int depth = 0;

        BitBoard fromSet;
        BitBoard occ = OccupiedSquares;
        BitBoard attackers;

        gain[depth++] = PieceValue[captured.Type];

        Color side = GetOpposite(sideToMove);
        occ ^= SquareMask[from];

        attackers = AttacksTo(to, side, occ) & occ;

        while(attackers)
        {
            gain[depth] = PieceValue[attackingPiece] - gain[depth - 1];

            std::pair<BitBoard, Type> tuple = LeastValuableAttacker(side, attackers);
            fromSet = tuple.first;
            attackingPiece = tuple.second;

            occ ^= fromSet;
            side = GetOpposite(side);
            attackers = AttacksTo(to, side, occ) & occ;
            depth++;
        }

        while (--depth)
        {
            gain[depth-1] = -std::max(-gain[depth-1], gain[depth]);
        }

        return gain[0];
    }

    inline bool Board::IsRepetition() const
    {
        if (halfMoveClock >= 4)
        {
            int start = SideToMove() == PieceColor::White ? 0 : 1;
            for (int i = start; i<currentPly; i+=2)
            {
                if (hashHistory[i] == zobrist)
                    return true;
            }
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

        if (PlayerPieces() & EnemyPieces())
            return false;

        if (playerPieces & enemyPieces)
            return false;

        if (playerPieces != PlayerPieces())
            return false;

        if (enemyPieces != EnemyPieces())
            return false;

        if ((playerPieces | enemyPieces) != OccupiedSquares)
            return false;

        if (pieceSet[kingSquare[PieceColor::White]].Color != PieceColor::White)
            return false;

        if (pieceSet[kingSquare[PieceColor::Black]].Color != PieceColor::Black)
            return false;

        return true;
    }
}

#endif // BOARD_H

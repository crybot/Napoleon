#include "move.h"
#include "utils.h"
#include "piece.h"
#include "constants.h"
#include "board.h"

namespace Napoleon
{
    bool Move::IsNull() const
    {
        return (FromSquare() == ToSquare());
    }

    bool Move::IsCastleOO() const
    {
        return ((move >> 12) == KingCastle);
    }

    bool Move::IsCastleOOO() const
    {
        return ((move >> 12) == QueenCastle);
    }

    // convert move object in long algebraic notation as used by uci protocol
    std::string Move::ToAlgebraic() const
    {
        std::string algebraic;

        if (IsNull())
            return "0000"; // UCI representation for NULL move

        if (IsCastle())
        {
            if (IsCastleOO())
            {
                if (FromSquare() == Constants::Squares::IntE1)
                    algebraic += "e1g1";
                else
                    algebraic += "e8g8";
            }

            else if (IsCastleOOO())
            {
                if (FromSquare() == Constants::Squares::IntE1)
                    algebraic += "e1c1";
                else
                    algebraic += "e8c8";
            }
        }

        else
        {
            algebraic += Utils::Square::ToAlgebraic(FromSquare());
            algebraic += Utils::Square::ToAlgebraic(ToSquare());

            if (IsPromotion())
                algebraic += Utils::Piece::GetInitial(PiecePromoted());
        }
        return algebraic;
    }

    // convert move object to Standard algebraic notation as used in most programs
    std::string Move::ToSan(Board& board) const
    {
        if (IsCastle())
        {
            if (IsCastleOO())
                return "O-O";
            else
                return "O-O-O";
        }

        std::string san = "";

        Square from = FromSquare();
        Square to = ToSquare();

        int rank = Utils::Square::GetRankIndex(from);
        int file = Utils::Square::GetFileIndex(from);
        bool sameRank = false;
        bool sameFile = false;

        Color side = board.SideToMove();
        BitBoard occ = board.OccupiedSquares;
        Piece piece = board.PieceOnSquare(from);

        if (piece.Type != PieceType::Pawn)
        {
            san += std::toupper(Utils::Piece::GetInitial(piece));

            BitBoard attackers = (board.MovesTo(to, side, occ) & board.Pieces(side, piece.Type)) ^ Constants::Masks::SquareMask[from];
            BitBoard others = attackers;

            while(attackers)
            {
                Square square = Utils::BitBoard::BitScanForwardReset(attackers);
                Move move(square, to);

                if (!board.IsMoveLegal(move, board.PinnedPieces()))
                {
                    others ^= square;
                    continue;
                }


                if (Utils::Square::GetRankIndex(square) == rank)
                    sameRank = true;
                else if (Utils::Square::GetFileIndex(square) == file)
                    sameFile = true;
            }

            //        Ambiguities
            //        If the piece is sufficient to unambiguously determine the origin square, the whole from square is omitted. Otherwise,
            //        if two (or more) pieces of the same kind can move to the same square, the piece's initial is followed by
            //        (in descending order of preference):
            //
            //           1. file of departure if different
            //           2. rank of departure if the files are the same but the ranks differ
            //           3. the complete origin square coordinate otherwise
            if (others)
            {
                //        std::cout << sameRank << " " << sameFile << std::endl;
                //        assert(sameRank != true && sameFile != true);
                //        assert((sameRank ^ sameFile) || (!sameRank && !sameFile));

                if (!sameFile)
                    san += Utils::Square::ToAlgebraic(from)[0]; //  + file letter
                else if (!sameRank)
                    san += Utils::Square::ToAlgebraic(from)[1]; //  + rank number
                else
                    san += Utils::Square::ToAlgebraic(from);
            }
        }
        else if (board.IsCapture(*this))
            san = Utils::Square::ToAlgebraic(from)[0];


        if (board.IsCapture(*this))
            san += "x";

        san += Utils::Square::ToAlgebraic(ToSquare());

        if (IsPromotion())
            san += "=" + std::toupper(Utils::Piece::GetInitial(PiecePromoted()));

        return san;
    }

}

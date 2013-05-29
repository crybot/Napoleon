#include "move.h"
#include "utils.h"
#include "piece.h"
#include "constants.h"

namespace Napoleon
{
    bool MoveEncode::IsNull(Move move)
    {
        return (FromSquare(move) == ToSquare(move));
    }

    bool MoveEncode::IsCastleOO(Move move)
    {
        return (FromSquare(move) == 60 && ToSquare(move) == 62) || (FromSquare(move) == 4 && ToSquare(move) == 6);
    }

    bool MoveEncode::IsCastleOOO(Move move)
    {
        return (FromSquare(move) == 60 && ToSquare(move) == 58) || (FromSquare(move) == 4 && ToSquare(move) == 2);
    }

    std::string MoveEncode::ToAlgebraic(Move move)
    {
        std::string algebraic;

        if (IsNull(move))
            return "0000";

        if (IsCastle(move))
        {
            if (IsCastleOO(move))
            {
                if (FromSquare(move) == Constants::Squares::IntE1)
                    algebraic += "e1g1";
                else
                    algebraic += "e8g8";
            }

            else if (IsCastleOOO(move))
            {
                if (FromSquare(move) == Constants::Squares::IntE1)
                    algebraic += "e1c1";
                else
                    algebraic += "e8c8";
            }
        }

        else
        {
            //            algebraic += Utils::Piece::GetInitial(PieceMoved);
            algebraic += Utils::Square::ToAlgebraic(FromSquare(move));

            //            if (IsCapture())
            //                algebraic += "x";

            algebraic += Utils::Square::ToAlgebraic(ToSquare(move));

            if (IsPromotion(move))
                algebraic += Utils::Piece::GetInitial(PiecePromoted(move));

            //            else if (IsEnPassant())
            //                algebraic += "e.p.";
        }

        return algebraic;
    }

}

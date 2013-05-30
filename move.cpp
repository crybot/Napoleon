#include "move.h"
#include "utils.h"
#include "piece.h"
#include "constants.h"

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

}

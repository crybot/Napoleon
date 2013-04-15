#include "move.h"
#include "utils.h"
#include "piece.h"

namespace Napoleon
{

    bool Move::IsNull() const
    {
        return (FromSquare == ToSquare);
    }

    bool Move::IsCastleOO() const
    {
        return (FromSquare == 60 && ToSquare == 62) || (FromSquare == 4 && ToSquare == 6);
    }

    bool Move::IsCastleOOO() const
    {
        return (FromSquare == 60 && ToSquare == 58) || (FromSquare == 4 && ToSquare == 2);
    }

    std::string Move::ToAlgebraic() const
    {
        std::string algebraic = "";

        if (IsCastle())
        {
            if (IsCastleOO())
                algebraic += "O-O";

            else if (IsCastleOOO())
                algebraic += "O-O-O";
        }

        else
        {
//            algebraic += Utils::Piece::GetInitial(PieceMoved);
            algebraic += Utils::Square::ToAlgebraic(FromSquare);

            if (IsCapture())
                algebraic += "x";

            algebraic += Utils::Square::ToAlgebraic(ToSquare);

            if (IsPromotion())
                algebraic += Utils::Piece::GetInitial(PiecePromoted);

            else if (IsEnPassant())
                algebraic += "e.p.";
        }

        return algebraic;
    }

}

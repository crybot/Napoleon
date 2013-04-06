#ifndef MOVE_H
#define MOVE_H
#include "defines.h"
#include "piece.h"

namespace Napoleon
{
    class Move
    {
    public:
        Byte FromSquare;
        Byte ToSquare;
        Byte PieceCaptured;
        Byte PiecePromoted;// overloaded to manage castle (ROOK) // overloaded to manage en-passant (PAWN)

        Move(int, int, Byte, Byte);
        Move();

        bool operator== (const Move&) const;
        bool operator!= (const Move&) const;
        bool IsNull() const;
        bool IsCapture() const;
        bool IsCastle() const;
        bool IsCastleOO() const;
        bool IsCastleOOO() const;
        bool IsPromotion() const;
        bool IsEnPassant() const;
        std::string ToAlgebraic() const;

    };

    INLINE Move::Move(int fromSquare, int toSquare, Byte pieceCaptured, Byte piecePromoted)
        :FromSquare(fromSquare), ToSquare(toSquare), PieceCaptured(pieceCaptured), PiecePromoted(piecePromoted) { }

    INLINE Move::Move() {}

    INLINE bool Move::IsCapture() const
    {
        return (PieceCaptured != PieceType::None);
    }

    INLINE bool Move::IsEnPassant() const
    {
        return (PiecePromoted == PieceType::Pawn);
    }
}
#endif // MOVE_H

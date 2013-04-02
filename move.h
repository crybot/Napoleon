#ifndef MOVE_H
#define MOVE_H
#include "defines.h"

namespace Napoleon
{
    class Move
    {
    public:
        Byte FromSquare;
        Byte ToSquare;
        Byte PieceMoved; // overloaded to manage castle (KING)
        Byte PieceCaptured;
        Byte PiecePromoted;// overloaded to manage castle (ROOK) // overloaded to manage en-passant (PAWN)

        Move(Byte, Byte, Byte, Byte, Byte);
        Move();

        void SetMove(Byte, Byte, Byte, Byte, Byte);

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

    INLINE Move::Move(Byte fromSquare, Byte toSquare, Byte pieceMoved, Byte pieceCaptured, Byte piecePromoted)
        :FromSquare(fromSquare), ToSquare(toSquare), PieceMoved(pieceMoved), PieceCaptured(pieceCaptured), PiecePromoted(piecePromoted) { }

    INLINE Move::Move() {}
}
#endif // MOVE_H

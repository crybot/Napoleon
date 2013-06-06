#ifndef MOVE_H
#define MOVE_H
#include "defines.h"
#include "piece.h"

namespace Napoleon
{
    const MoveType KingCastle = 0x2, QueenCastle = 0x3, EnPassant = 0x5, QueenPromotion = 0xB,
    RookPromotion = 0xA, BishopPromotion = 0x9, KnightPromotion = 0x8;

    class Move
    {
    public:
        Move();
        Move(Square, Square);
        Move(Square, Square, Square);

        Square FromSquare() const;
        Square ToSquare() const;
        Square PiecePromoted() const;

        int ButterflyIndex() const;
        bool IsNull() const;
        bool IsCastle() const;
        bool IsCastleOO() const;
        bool IsCastleOOO() const;
        bool IsPromotion() const;
        bool IsEnPassant() const;
        bool operator== (const Move&) const;
        bool operator!= (const Move&) const;
        std::string ToAlgebraic() const;

    private:
        unsigned short move;

    };

    INLINE Move::Move() { }

    inline Move::Move(Square from, Square to)
    {
        move = (from & 0x3f) | ((to & 0x3f) << 6);
    }

    inline Move::Move(Square from, Square to, Square flag)
    {
        move = (from & 0x3f) | ((to & 0x3f) << 6) | ((flag & 0xf) << 12);
    }

    inline Square Move::FromSquare() const
    {
        return move & 0x3f;
    }

    inline Square Move::ToSquare() const
    {
        return (move >> 6) & 0x3f;
    }

    inline int Move::ButterflyIndex() const // used to index from-to based tables
    {
        return (move & 0xfff);
    }

    inline Square Move::PiecePromoted() const
    {
        if (!IsPromotion())
            return PieceType::None;

        return ((move >> 12) & 0x3) + 1;
    }

    inline bool Move::IsEnPassant() const
    {
        return ((move >> 12) == EnPassant); // e.p. are encoded 0101
    }

    inline bool Move::IsCastle() const
    {
        return (((move >> 12) == KingCastle) || ((move >> 12) == QueenCastle));
    }

    inline bool Move::IsPromotion() const
    {
        return ((move >> 12) & 0x8);
    }

    inline bool Move::operator ==(const Move& other) const
    {
        return (move == other.move);
    }

    inline bool Move::operator !=(const Move& other) const
    {
        return (move != other.move);
    }
}
#endif // MOVE_H

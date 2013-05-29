#ifndef MOVE_H
#define MOVE_H
#include "defines.h"
#include "piece.h"

namespace Napoleon
{
    typedef unsigned short Move;

    namespace MoveEncode
    {
        Move CreateMove(int, int, int);

        int FromSquare(Move);
        int ToSquare(Move);
        int PiecePromoted(Move);

        bool IsNull(Move);
        bool IsCastle(Move);
        bool IsCastleOO(Move);
        bool IsCastleOOO(Move);
        bool IsPromotion(Move);
        bool IsEnPassant(Move);
        std::string ToAlgebraic(Move);

    }

    inline Move MoveEncode::CreateMove(int from, int to, int flags)
    {
        return (from & 0x3f) | ((to & 0x3f) << 6) | ((flags & 0xf) << 12);
    }

    inline int MoveEncode::FromSquare(Move move)
    {
        return move & 0x3f;
    }

    inline int MoveEncode::ToSquare(Move move)
    {
        return (move >> 6) & 0x3f;
    }

    inline int MoveEncode::PiecePromoted(Move move)
    {
        if (!IsPromotion(move))
            return PieceType::None;

        return ((move >> 12) & 0x3) + 1;
    }

    inline bool MoveEncode::IsEnPassant(Move move)
    {
        return ((move >> 12) == 0x5); // e.p. are encoded 0101
    }

    inline bool MoveEncode::IsCastle(Move move)
    {
        return (((move >> 12) == 0x2) || ((move >> 12) == 0x3));
    }

    inline bool MoveEncode::IsPromotion(Move move)
    {
        return ((move >> 12) & 0x8);
    }


}
#endif // MOVE_H

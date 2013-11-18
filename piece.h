#ifndef PIECE_H
#define PIECE_H
#include "defines.h"

namespace Napoleon
{
    // cannot convert to strongly-typed c++11 enums because we need implicit conversion
    namespace PieceType
    {
        const Type Pawn = 0;
        const Type Knight = 1;
        const Type Bishop = 2;
        const Type Rook = 3;
        const Type Queen = 4;
        const Type King = 5;
        const Type None = 6;
    }

    // cannot convert to strongly-typed c++11 enums because we need implicit conversion
    namespace PieceColor
    {
        const Color White = 0;
        const Color Black = 1;
        const Color None = 2;
    }

    class Piece
    {
    public:
        Byte Color;
        Byte Type;
        Piece(Byte, Byte);
        Piece();
    };
}

#endif // PIECE_H

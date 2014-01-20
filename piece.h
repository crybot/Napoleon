#ifndef PIECE_H
#define PIECE_H
#include "defines.h"

namespace Napoleon
{
    // cannot convert to strongly-typed c++11 enums because we need implicit conversion
    enum PieceType : Type
    {
        Pawn, Knight, Bishop, Rook, Queen, King, None
    };

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

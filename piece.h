#ifndef PIECE_H
#define PIECE_H
#include "defines.h"
namespace Napoleon
{
    struct PieceType
    {
        static const Byte Pawn = 0;
        static const Byte Knight = 1;
        static const Byte Bishop = 2;
        static const Byte Rook = 3;
        static const Byte Queen = 4;
        static const Byte King = 5;
        static const Byte None = 6;
    };

    struct PieceColor
    {
        static const Byte White = 0;
        static const Byte Black = 1;
        static const Byte None = 2;
    };

    class Piece
    {
    public:
        Byte Type;
        Byte Color;
        Piece(Byte,Byte);
        Piece();
    };
}

#endif // PIECE_H

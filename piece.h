#ifndef PIECE_H
#define PIECE_H
#include "defines.h"

namespace Napoleon
{
    namespace PieceType
    {
        const Byte Pawn = 0;
        const Byte Knight = 1;
        const Byte Bishop = 2;
        const Byte Rook = 3;
        const Byte Queen = 4;
        const Byte King = 5;
        const Byte None = 6;
    }

    namespace PieceColor
    {
        const Byte White = 0;
        const Byte Black = 1;
        const Byte None = 2;
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

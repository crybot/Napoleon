#include "piece.h"

namespace Napoleon
{
    Piece::Piece(Byte color, Byte type)
        :Color(color), Type(type) { }

    Piece::Piece()
        :Color(PieceColor::None), Type(PieceType::None) { }
}

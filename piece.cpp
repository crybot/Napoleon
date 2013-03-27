#include "piece.h"

namespace Napoleon
{
    Piece::Piece(Byte type, Byte color)
        :Type(type), Color(color) { }

    Piece::Piece()
        :Type(PieceType::None), Color(PieceColor::None) { }
}

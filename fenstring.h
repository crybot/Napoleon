#ifndef FENSTRING_H
#define FENSTRING_H
#include <string>
#include "defines.h"
#include "piece.h"

namespace Napoleon
{
    class FenString
    {
    public:
        std::string FullString;
        Piece PiecePlacement[64];
        Byte SideToMove;
        bool CanWhiteShortCastle;
        bool CanWhiteLongCastle;
        bool CanBlackShortCastle;
        bool CanBlackLongCastle;
        int EnPassantSquare;

        FenString(std::string);

        void Parse();

    private:
        void parsePiecePlacement(std::string);
        void parseSideToMove(std::string);
        void parseCastling(std::string);
        void parseEnPassant(std::string);
    };

}
#endif // FENSTRING_H

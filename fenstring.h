#ifndef FENSTRING_H
#define FENSTRING_H
#include <string>
#include "defines.h"
#include "piece.h"
#include "move.h"
namespace Napoleon
{
    class FenString
    {
    public:
        std::string FullString;
        Piece PiecePlacement[64];
        Byte sideToMove;
        bool CanWhiteShortCastle;
        bool CanWhiteLongCastle;
        bool CanBlackShortCastle;
        bool CanBlackLongCastle;
        int EnPassantSquare;
        int HalfMove;
        std::string BestMove;

        FenString(std::string);

        void Parse();

    private:
        void parsePiecePlacement(std::string);
        void parsesideToMove(std::string);
        void parseCastling(std::string);
        void parseEnPassant(std::string);
        void parseHalfMove(std::string);
        void parseBestMove(std::string);
    };

}
#endif // FENSTRING_H

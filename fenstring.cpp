#include "fenstring.h"
#include "utils.h"
#include "constants.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

namespace Napoleon
{
    FenString::FenString(std::string str)
        :FullString(str)
    {
        Parse();
    }

    void FenString::Parse()
    {
        std::vector<std::string> fields;
        boost::split(fields, FullString, boost::is_any_of(" "));

        std::string piecePlacement = fields[0];
        std::string sideToMove = fields[1];
        std::string castling = fields[2];
        std::string enPassant = fields[3];

        if (fields.size() == 6)
        {
            std::string halfMove = fields[4];
            std::string fullMove = fields[5];
        }

        parsePiecePlacement(piecePlacement);
        parseSideToMove(sideToMove);
        parseCastling(castling);
        parseEnPassant(enPassant);
    }

    void FenString::parsePiecePlacement(std::string field)
    {
        for (int i=0; i<64; i++)
        {
            PiecePlacement[i] = Piece(PieceColor::None, PieceType::None);
        }

        std::vector<std::string> ranks;
        boost::split(ranks, field, boost::is_any_of("/"));

        for (unsigned i = 0; i < ranks.size(); i++)
        {
            int empty = 0;
            for (unsigned l = 0; l < ranks[i].size(); l++)
            {
                switch (ranks[i][l])
                {
                case 'P':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::White, PieceType::Pawn);
                    break;
                case 'p':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::Black, PieceType::Pawn);
                    break;
                case 'N':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::White, PieceType::Knight);
                    break;
                case 'n':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::Black, PieceType::Knight);
                    break;
                case 'B':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::White, PieceType::Bishop);
                    break;
                case 'b':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::Black, PieceType::Bishop);
                    break;
                case 'R':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::White, PieceType::Rook);
                    break;
                case 'r':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::Black, PieceType::Rook);
                    break;
                case 'Q':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::White, PieceType::Queen);
                    break;
                case 'q':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::Black, PieceType::Queen);
                    break;
                case 'K':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::White, PieceType::King);
                    break;
                case 'k':
                    PiecePlacement[Utils::Square::GetSquareIndex(l + empty, 7 - i)] = Piece(PieceColor::Black, PieceType::King);
                    break;
                default:
                    empty += (boost::lexical_cast<int>(ranks[i][l]) - 1);
                    break;
                }
            }
        }

        return;
    }

    void FenString::parseSideToMove(std::string field)
    {
        switch (field[0])
        {
        case 'w':
            SideToMove = PieceColor::White;
            break;
        case 'b':
            SideToMove = PieceColor::Black;
            break;
        }
    }

    void FenString::parseCastling(std::string field)
    {
        CanWhiteShortCastle = false;
        CanWhiteLongCastle = false;
        CanBlackShortCastle = false;
        CanBlackLongCastle = false;

        for (unsigned i = 0; i < field.size(); i++)
        {
            switch (field[i])
            {
            case 'K':
                CanWhiteShortCastle = true;
                break;
            case 'k':
                CanBlackShortCastle = true;
                break;
            case 'Q':
                CanWhiteLongCastle = true;
                break;
            case 'q':
                CanBlackLongCastle = true;
                break;
            }
        }
    }

    void FenString::parseEnPassant(std::string field)
    {
        if (field.size() == 1)
        {
            if (field[0] == '-')
                EnPassantSquare = Constants::Squares::Invalid;
        }
        else
        {
            EnPassantSquare = Utils::Square::Parse(field);
        }
    }
}

#include "fenstring.h"
#include "utils.h"
#include "constants.h"
#include <boost/algorithm/string.hpp>
//#include <boost/lexical_cast.hpp>
#include <vector>
#include <sstream>

namespace Napoleon
{
    FenString::FenString(std::string str)
        :FullString(str)
    {
        Parse();
    }

    void FenString::Parse()
    {
        std::istringstream stream(FullString);

        std::string piecePlacement;
        std::string sideToMove;
        std::string castling;
        std::string enPassant;
        std::string halfMove;
        std::string fullMove;
        std::string bestMove;

        stream >> piecePlacement;
        stream >> sideToMove;
        stream >> castling;
        stream >> enPassant;

        std::string token;
        stream >> token;

        if (token == "bm") // EPD string
        {
            stream >> bestMove;
        }
        else
        {
            halfMove = token;
            stream >> fullMove;
        }

        parsePiecePlacement(piecePlacement);
        parsesideToMove(sideToMove);
        parseCastling(castling);
        parseEnPassant(enPassant);

        if (!halfMove.empty())
            parseHalfMove(halfMove);

        if (!bestMove.empty())
            parseBestMove(bestMove);
    }

    void FenString::parsePiecePlacement(std::string field)
    {
        for (int i=0; i<64; i++)
        {
            PiecePlacement[i] = Constants::Piece::Null;
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
                    empty += std::stoi(std::string(&ranks[i][l]))-1; // TO TEST

                    //                    empty += (boost::lexical_cast<int>(ranks[i][l]) - 1);
                    break;
                }
            }
        }

        return;
    }

    void FenString::parsesideToMove(std::string field)
    {
        switch (field[0])
        {
        case 'w':
            sideToMove = PieceColor::White;
            break;
        case 'b':
            sideToMove = PieceColor::Black;
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

    void FenString::parseHalfMove(std::string field)
    {
        HalfMove = std::stoi(field);
    }

    void FenString::parseBestMove(std::string field)
    {
        BestMove = field;

        //we don't care if the move gives check or mate (for now)
        if (field.back() == '+' || field.back() == '#')
            BestMove = field.substr(0, field.size()-1);
    }


}

#include "utils.h"
#include "piece.h"
#include "constants.h"
#include "console.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace Napoleon
{
    namespace Utils
    {
        namespace Square
        {
            int Parse(std::string square)
            {
                // convert algebraic notation (e.g. a1) in orthogonal coordinates
                std::transform(square.begin(), square.end(), square.begin(), ::tolower); // convert string in lower case
                int x = static_cast<int>(square[0] - 'a');
                int y = static_cast<int>(square[1] - '1');
                return GetSquareIndex(x, y);
            }

            std::string ToAlgebraic(Napoleon::Square square)
            {
                if (square == Constants::Squares::Invalid)
                    return "Invalid";

                std::string str = "";
                str += static_cast<char>(GetFileIndex(square) + 'a');
                str += std::to_string(GetRankIndex(square) + 1);

                return str;
            }
        }

        namespace BitBoard
        {
            bool IsBitSet(Napoleon::BitBoard bitBoard, int bitPos)
            {
                return (bitBoard & (static_cast<Napoleon::BitBoard>(1) << bitPos)) != 0;
            }

            void Display(Napoleon::BitBoard bitBoard)
            {
                for (int r = 7; r >= 0; r--)
                {
                    std::cout << "   ------------------------\n";

                    std::cout << " " << r + 1 << " ";

                    for (int c = 0; c <= 7; c++)
                    {
                        std::cout << '[';
                        if (IsBitSet(bitBoard, Square::GetSquareIndex(c, r)))
                        {
                            std::cout /*<< Console::Green*/ << '1';
                        }
                        else
                        {
                            std::cout /*<< Console::Red*/ << '0';
                        }

                        std::cout /*<< Console::Reset*/ << ']';
                    }
                    std::cout << std::endl;
                }
                std::cout << "\n    A  B  C  D  E  F  G  H\n";
            }

            std::string ToString(Napoleon::BitBoard bitBoard)
            {
                std::string flattened;
                for (int r = 7; r >= 0; r--)
                {
                    for (int c = 0; c <= 7; c++)
                    {
                        if (IsBitSet(bitBoard, Square::GetSquareIndex(c, r)))
                        {
                            flattened += "1";
                        }
                        else
                        {
                            flattened += "0";
                        }
                    }
                }
                return flattened;
            }
        }

        namespace Piece
        {
            Byte GetPiece(char initial)
            {
                switch(initial)
                {
                case 'b':
                    return PieceType::Bishop;
                case 'n':
                    return PieceType::Knight;
                case 'q':
                    return PieceType::Queen;
                case 'r':
                    return PieceType::Rook;
                default:
                    throw std::exception(); // TODO
                }
            }

            char GetInitial(Byte type)
            {
                switch (type)
                {
                case PieceType::Bishop:
                    return 'b';
                case PieceType::King:
                    return 'k';
                case PieceType::Knight:
                    return 'n';
                case PieceType::Pawn:
                    return 'p';
                case PieceType::Queen:
                    return 'q';
                case PieceType::Rook:
                    return 'r';
                default:
                    throw std::exception(); // TODO
                }
            }

            char GetInitial(Napoleon::Piece piece)
            {
                if (piece.Color == PieceColor::White)
                {
                    switch (piece.Type)
                    {
                    case PieceType::Bishop:
                        return 'B';
                    case PieceType::King:
                        return 'K';
                    case PieceType::Knight:
                        return 'N';
                    case PieceType::Pawn:
                        return 'P';
                    case PieceType::Queen:
                        return 'Q';
                    case PieceType::Rook:
                        return 'R';
                    default:
                        throw std::exception(); // TODO
                    }
                }
                else
                {
                    switch (piece.Type)
                    {
                    case PieceType::Bishop:
                        return 'b';
                    case PieceType::King:
                        return 'k';
                    case PieceType::Knight:
                        return 'n';
                    case PieceType::Pawn:
                        return 'p';
                    case PieceType::Queen:
                        return 'q';
                    case PieceType::Rook:
                        return 'r';
                    default:
                        throw std::exception(); // TODO
                    }
                }
            }
        }

        namespace Math
        {
            double Log2(double x)
            {
                return std::log(x) / std::log(2.);
            }
        }

        namespace String
        {

            void Split(std::vector<std::string> &results, const std::string &str, char sep)
            {
                std::string::const_iterator first = str.begin();
                std::string::const_iterator last  = str.end();
                std::string::const_iterator pos;

                while(first != last)
                {
                    pos = std::find(first, last, sep);
                    results.push_back(std::string(first, pos));

                    // If no separator was found, we can stop our search here
                    if(pos == last) break;

                    // Ignore separator
                    first = pos + 1;
                }
            }
        }

    }
}


#include "utils.h"
#include "piece.h"
#include "constants.h"
#include "console.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include <boost/lexical_cast.hpp>

namespace Napoleon
{
    namespace Utils
    {
        namespace Square
        {
            int Parse(std::string square)
            {
                // converte la notazione algebrica (es. a1) in coordinate decimali
                std::transform(square.begin(), square.end(), square.begin(), ::tolower); // converte la stringa in minuscolo
                int x = (int)(square[0] - 'a');
                int y = (int)(square[1] - '1');
                return GetSquareIndex(x, y);
            }

            std::string ToAlgebraic(int square)
            {
                if (square == Constants::Squares::Invalid)
                    return "Invalid";

                std::string str = "";
                str += (char)(GetFileIndex(square) + 97);
                str += boost::lexical_cast<std::string>(GetRankIndex(square) + 1);

                return str;
            }
        }

        namespace BitBoard
        {
            bool IsBitSet(Napoleon::BitBoard bitBoard, int posBit)
            {
                return (bitBoard & ((Napoleon::BitBoard)1 << (posBit))) != 0;
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
                            std::cout << Console::Green << '1';
                        }
                        else
                        {
                            std::cout << Console::Red << '0';
                        }

                        std::cout << Console::Reset << ']';
                    }
                    std::cout << std::endl;
                }
                std::cout << "\n    A  B  C  D  E  F  G  H\n";
            }

            int PopCount(Napoleon::BitBoard bitBoard)
            {
#ifdef __GNUG__
                return __builtin_popcount(bitBoard);
#else
                bitBoard -= ((bitBoard >> 1) & 0x5555555555555555UL);
                bitBoard = ((bitBoard >> 2) & 0x3333333333333333UL) + (bitBoard & 0x3333333333333333UL);
                bitBoard = ((bitBoard >> 4) + bitBoard) & 0x0F0F0F0F0F0F0F0FUL;
                return (int)((bitBoard * 0x0101010101010101UL) >> 56);
#endif
            }

        }

        namespace Piece
        {
            char GetInitial(Byte type)
            {
                switch (type)
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
        }
    }
}


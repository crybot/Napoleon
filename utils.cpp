#include "utils.h"
#include "piece.h"
#include "console.h"
#include <cassert>

namespace Napoleon
{
    namespace Utils
    {
        namespace Square
        {
            int GetA1H8DiagonalIndex(int file, int rank)
            {
                return 7 + rank - file;
            }

            int GetA1H8DiagonalIndex(int squareIndex)
            {
                return 7 + GetRankIndex(squareIndex) - GetFileIndex(squareIndex);
            }

            int GetH1A8AntiDiagonalIndex(int file, int rank)
            {
                return rank + file;
            }

            int GetH1A8AntiDiagonalIndex(int squareIndex)
            {
                return GetRankIndex(squareIndex) + GetFileIndex(squareIndex);
            }

            int GetFileIndex(int squareIndex)
            {
                return squareIndex & 7;
            }

            int GetRankIndex(int squareIndex)
            {
                return squareIndex >> 3;
            }

            int GetSquareIndex(int file, int rank)
            {
                return file + 8 * rank;
            }

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
                std::string str = "";
                str += (char)(GetFileIndex(square) + 97);
                str += (int)(GetRankIndex(square) + 1);

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
                bitBoard -= ((bitBoard >> 1) & 0x5555555555555555UL);
                bitBoard = ((bitBoard >> 2) & 0x3333333333333333UL) + (bitBoard & 0x3333333333333333UL);
                bitBoard = ((bitBoard >> 4) + bitBoard) & 0x0F0F0F0F0F0F0F0FUL;
                return (int)((bitBoard * 0x0101010101010101UL) >> 56);
            }

            int BitScanForward(Napoleon::BitBoard bitBoard)
            {
                assert(bitBoard != 0);
                return Constants::DeBrujinTable[((bitBoard & -bitBoard) * Constants::DeBrujinValue) >> 58];
            }

            int BitScanForwardReset(Napoleon::BitBoard& bitBoard)
            {
                assert(bitBoard != 0);
                Napoleon::BitBoard bb = bitBoard;
                bitBoard &= (bitBoard - 1);

                return Constants::DeBrujinTable[((bb & -bb) * Constants::DeBrujinValue) >> 58];
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

            char GetOpposite(Byte color)
            {
                if (color == PieceColor::White)
                    return PieceColor::Black;
                else if (color == PieceColor::Black)
                    return PieceColor::White;
                else
                    throw std::exception(); // TODO
            }
        }
    }
}


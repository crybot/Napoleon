#ifndef UTILS_H
#define UTILS_H
#include "defines.h"
#include <string>

namespace Napoleon
{
    namespace Utils
    {
        namespace Square
        {
            int GetA1H8DiagonalIndex(int, int);
            int GetA1H8DiagonalIndex(int);
            int GetH1A8AntiDiagonalIndex(int, int);
            int GetH1A8AntiDiagonalIndex(int);
            int GetFileIndex(int);
            int GetRankIndex(int);
            int GetSquareIndex(int, int);
            int Parse(std::string);
            std::string ToAlgebraic(int);
        }

        namespace BitBoard
        {
            bool IsBitSet(Napoleon::BitBoard, int);
            void Display(Napoleon::BitBoard);

            int PopCount(Napoleon::BitBoard bitBoard);
            int BitScanForward( Napoleon::BitBoard bitBoard);
            int BitScanForwardReset(Napoleon::BitBoard& bitBoard);
        }

        namespace Piece
        {
            char GetInitial(Byte);
            char GetOpposite(Byte);
        }

        __always_inline int Square::GetA1H8DiagonalIndex(int file, int rank)
        {
            return 7 + rank - file;
        }

        __always_inline int Square::GetA1H8DiagonalIndex(int squareIndex)
        {
            return 7 + GetRankIndex(squareIndex) - GetFileIndex(squareIndex);
        }

        __always_inline int Square::GetH1A8AntiDiagonalIndex(int file, int rank)
        {
            return rank + file;
        }

        __always_inline int Square::GetH1A8AntiDiagonalIndex(int squareIndex)
        {
            return GetRankIndex(squareIndex) + GetFileIndex(squareIndex);
        }

        __always_inline int Square::GetFileIndex(int squareIndex)
        {
            return squareIndex & 7;
        }

        __always_inline int Square::GetRankIndex(int squareIndex)
        {
            return squareIndex >> 3;
        }

        __always_inline int Square::GetSquareIndex(int file, int rank)
        {
            return file + 8 * rank;
        }

    }
}
#endif // UTILS_H

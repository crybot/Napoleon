#ifndef UTILS_H
#define UTILS_H
#include "constants.h"
#include "defines.h"
#include "piece.h"

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
            int BitScanForward(Napoleon::BitBoard bitBoard);
            int BitScanForwardReset(Napoleon::BitBoard& bitBoard);
        }

        namespace Piece
        {
            char GetInitial(Byte);
            char GetOpposite(Byte);
        }
    }
}
#endif // UTILS_H

#ifndef EVALUATION_H
#define EVALUATION_H
#include "defines.h"
#include "constants.h"
#include "utils.h"
#include "piecesquaretables.h"
#include <cassert>

namespace Napoleon
{
    class Piece;
    namespace Evaluation
    {
        int Evaluate(Board&);
        int EvaluatePiece(Piece, Square, Board&);
        Score PieceSquareValue(Piece, Square);
        inline void updateScore(Score&, int, int);
        inline void updateScore(Score&, int);

        extern int multiPawnP[8]; // penalization for doubled, tripled... pawns
        extern int mobilityBonus[][Constants::QueenMaxMoves + 1];
    }

    INLINE void Evaluation::updateScore(std::pair<int, int>& scores, int openingBonus, int endBonus)
    {
        scores.first += openingBonus;
        scores.second += endBonus;
    }
    INLINE void Evaluation::updateScore(std::pair<int, int>& scores, int openingBonus)
    {
        scores.first += openingBonus;
        scores.second += openingBonus;
    }

    inline Score Evaluation::PieceSquareValue(Piece piece, Square square)
    {
        square = piece.Color == PieceColor::White ? square : Utils::Square::MirrorSquare(square);

        return std::make_pair(PieceSquareTable[piece.Type][Opening][square], PieceSquareTable[piece.Type][EndGame][square]);
    }
}

#endif // EVALUATION_H

#ifndef EVALUATION_H
#define EVALUATION_H
#include "defines.h"
#include "constants.h"
#include "utils.h"
#include "king.h"
#include "piecesquaretables.h"
#include "compassrose.h"
#include "board.h"
#include <cassert>

namespace Napoleon
{
    class Piece;
    namespace Evaluation
    {
        int Evaluate(Board&);
        Score EvaluatePiece(Piece, Square, BitBoard, Board&);
        Score PieceSquareValue(Piece, Square);
        int KingSafety(Board&);

        void PrintEval(Board&);

        int interpolate(Score, int);
        void formatParam(std::string, int, int);
        void formatParam(std::string, Score, Score, int);
        inline void updateScore(Score&, int, int);
        inline void updateScore(Score&, int);
        inline void updateScore(Score&, Score);


        extern int multiPawnP[8]; // penalization for doubled, tripled... pawns
        extern int isolatedPawnP[8];
        extern int passedPawn[3][8]; // phase, file
        extern int candidatePawn[3][8]; // phase, color, rank
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
    INLINE void Evaluation::updateScore(std::pair<int, int>& scores, Score bonus)
    {
        scores.first += bonus.first;
        scores.second += bonus.second;
    }

    inline int Evaluation::interpolate(Score score, int phase)
    {
        return ((score.first * (Constants::Eval::MaxPhase - phase)) + (score.second * phase)) / Constants::Eval::MaxPhase; // linear-interpolated score
    }

    inline Score Evaluation::PieceSquareValue(Piece piece, Square square)
    {
        square = piece.Color == PieceColor::White ? square : Utils::Square::MirrorSquare(square);

        return std::make_pair(PieceSquareTable[piece.Type][Opening][square], PieceSquareTable[piece.Type][EndGame][square]);
    }
}

#endif // EVALUATION_H

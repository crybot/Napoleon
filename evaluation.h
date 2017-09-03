#ifndef EVALUATION_H
#define EVALUATION_H
#include "defines.h"
#include "constants.h"
#include "utils.h"
#include "king.h"
#include "piecesquaretables.h"
#include "compassrose.h"
#include "board.h"
#include "pawntable.h"
#include <cassert>
#include <cmath>

namespace Napoleon
{
    class Piece;
    namespace Evaluation
    {
        int Evaluate(Board&);
        Score EvaluatePiece(Piece, Square, BitBoard, const PawnEntry* const, Board&);
        Score PieceSquareValue(Piece, Square);
        int KingSafety(Board&);

        void PrintEval(Board&);

        Score evaluatePawn(Color, Square, Board&, PawnEntry*);
        int interpolate(Score, int);
        void formatParam(std::string, int, int);
        void formatParam(std::string, Score, Score, int);
        inline void updateScore(Score&, int, int);
        inline void updateScore(Score&, int);
        inline void updateScore(Score&, Score);


        extern int multiPawnP[8]; // penalization for doubled, tripled... pawns
        extern int isolatedPawnP[8]; // number
        extern int passedPawn[3][8]; // phase, file
        extern int candidatePawn[3][8]; // phase, rank
        extern int supportedPassed[3]; // phase
        extern int pawnIslandsP[3][5]; // phase, number
        extern int mobilityBonus[3][7][Constants::QueenMaxMoves + 1];
        extern int backwardPawnP[3]; // phase
        extern BitBoard pawnAttacks[2]; // color
        extern BitBoard unpinnedKnightAttacks[2]; // color
        extern thread_local PawnTable pawnTable;
        extern int hangingValue[2]; // color
        extern int kingAttacks[200]; // number of weighted attacks
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

    inline Score Evaluation::evaluatePawn(Color color, Square square, Board& board, PawnEntry* entry)
    {
        using namespace PieceColor;
        using namespace Utils::BitBoard;
        using namespace Utils::Square;

        Score score(0,0);

        BitBoard pawns[2] = { // color
            board.Pieces(White, Pawn), board.Pieces(Black, Pawn)
        };


        //int tropism = 1;

        Color enemy = Utils::Piece::GetOpposite(color);
        if ((MoveDatabase::FrontSpan[color][square] & pawns[color]) == 0) // NO OWN PAWNS IN FRONT
        {
            Rank rrank = RelativeRank(color, GetRankIndex(square));

            if ((MoveDatabase::PasserSpan[color][square] & pawns[enemy]) == 0) // NO ENEMY PAWNS (i.e. PASSED PAWN)
            {
                updateScore(score, passedPawn[Opening][rrank], passedPawn[EndGame][rrank]);
                //SetBit(entry->passers[color], square);
                //tropism = 2;
            }

            else if ((MoveDatabase::FrontSpan[color][square] & pawns[enemy]) == 0) // NO ENEMY PAWNS IN FRONT (i.e. OPEN PAWN)
            {
                int defenders = PopCount(MoveDatabase::CandidateDefenders[color][square] & pawns[color]);
                if (defenders > 0)
                {
                    int attackers = PopCount(MoveDatabase::AttackFrontSpan[color][square] & pawns[enemy]);

                    if (defenders >= attackers) // CANDIDATE PASSER
                    {
                        updateScore(score, candidatePawn[Opening][rrank], candidatePawn[EndGame][rrank]);
                        //SetBit(entry->passers[color], square);
                    }
                }

                /* BACKWARD PAWN DISABLED: no elo gains
                   if (rrank <= 4) // OPEN PAWNS ON SECOND, THIRD or FOURTH RANK (color relative)
                   {
                   BitBoard stop = MoveDatabase::PawnStop[color][square];

                // BACKWARD PAWN DEFINITION:
                // * - open pawn
                // * - pawn on second, third or fourth rank (color relative)
                // * - stop square attacked by enemy pawns
                // * - stop square not defended by own pawns
                // *   TODO: consider opposing backward pawns (the more advanced is not backward)
                // *   TODO: consider backward pawns for king-pawn tropism evaluation
                if ((stop & pawnAttacks[enemy]) && !(stop & pawnAttacks[color])) // BACKWARD PAWN
                {
                updateScore(score, -backwardPawnP[Opening], -backwardPawnP[EndGame]);
                }
                }
                */
            }
        }

        //int enemy_king_distance = MoveDatabase::Distance[square][board.KingSquare(enemy)];
        //int pawn_file = GetFileIndex(square);
        //int king_file = GetFileIndex(board.KingSquare(enemy));

        //pawn storm evaluation:
        //we penalize enemy pawns near own king
        //updateScore(score, 7 - enemy_king_distance, 0); 

        // NEXT TO TRY:
        // ONLY CONDIDER PAWNS ON ADJACENT FILES TO KING
        //if (std::abs(pawn_file - king_file) <= 1) // i.e. on the same or adjacent file to king
            //updateScore(score, 3 - enemy_king_distance/2, 0);

        return color == White ? score : -score;
    }


}

#endif // EVALUATION_H

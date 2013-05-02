#ifndef SEARCH_H
#define SEARCH_H
#include "defines.h"
#include "board.h"
#include "move.h"
#include "constants.h"
#include "movegenerator.h"
#include "constants.h"
#include "stopwatch.h"
#include <cstring>

namespace Napoleon
{
    class Board;
    class Move;
    class Search
    {
    public:

        static void IterativeSearch(Board&);
        static Move searchRoot(int, int, int, Board&);
        static int search(int, int, int, Board&);
        static int quiesce(int, int, Board&);

    private:
        static StopWatch Timer;
        static int ThinkTime;
        static int moveScores[];
        static int history[2][64][64];
        static Move killerMoves[][2];

        static void setScores(Move[], Board&, int, int);
        static void pickMove(Move[], int, int);
        static void orderCaptures(Move[], Board&, int, int);
    };

    inline void Search::IterativeSearch(Board& board)
    {
        Move move;
        Move best;
        Move toMake;
        Move pv[Constants::MaxPly];
        memset(history, 0, sizeof(history));
        delete[] board.Table.Table;
        board.Table.Table = new HashEntry[board.Table.Size];

        Timer.Start();
        ThinkTime = 1500;

        int i = 1;
        while(i<100 && Timer.Stop().ElapsedMilliseconds() < ThinkTime)
        {
            move = searchRoot(i++, -Infinity, Infinity, board);

            if (move != Constants::NullMove)
                toMake = move;
            //            pv[0] = toMake;

            //            board.MakeMove(toMake);
            //            std::cout << toMake.ToAlgebraic() << " ";

            //            int l;
            //            for (l=1; l<i; l++)
            //            {
            //                best = board.Table[board.zobrist % board.Table.Size].BestMove;

            //                if (best.IsNull())
            //                    break;

            //                pv[l] = best;

            //                assert(!best.IsNull());

            //                board.MakeMove(best);

            //                std::cout << best.ToAlgebraic() << " ";
            //            }

            //            for (int k=l-1; k>=0; k--)
            //            {
            //                assert(!pv[k].IsNull());

            //                board.UndoMove(pv[k]);
            //            }

            //            std::cout << std::endl;
        }

        std::cout << "A.I. Move: " << toMake.ToAlgebraic() << std::endl;
        board.MakeMove(toMake);
        board.Display();
    }

    INLINE Move Search::searchRoot(int depth, int alpha, int beta, Board& board)
    {
        int max = alpha;
        int pos = 0;
        int move = 0;
        int score;
        Move moves[Constants::MaxMoves];

        MoveGenerator::GetLegalMoves(moves, pos, board);

        for (int i=0; i<pos; i++)
        {
            if (Timer.Stop().ElapsedMilliseconds() >= ThinkTime)
                return Constants::NullMove;

            board.MakeMove(moves[i]);
            score = -Search::search(depth-1, alpha, beta, board);
            board.UndoMove(moves[i]);

            if( score > max )
            {
                max = score;
                move = i;
            }
        }

        return moves[move];
    }

}

#endif // SEARCH_H

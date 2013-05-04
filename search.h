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
        static int searchRoot(int, int, int, Move&, Board&);
        static int search(int, int, int, Board&);
        static int quiescence(int, int, Board&);

    private:
        static StopWatch Timer;
        static int ThinkTime;
        static int moveScores[];
        static int lastScore;
        static int history[2][64][64];
        static Move killerMoves[][2];

        static void setScores(Move[], Board&, int, int);
        static void pickMove(Move[], int, int);
        static void orderCaptures(Move[], Board&, int, int);
    };

    // iterative deepening
    inline void Search::IterativeSearch(Board& board)
    {
        Move move;
        Move best;
        Move toMake;
        Move pv[Constants::MaxPly];
        const int AspirationValue = 100;

        memset(history, 0, sizeof(history));
//        memset(board.Table.Table, 0, sizeof(HashEntry) * board.Table.Size);

        Timer.Start();
        ThinkTime = 1000;

        int val;
        int temp;
        int i = 1;

        val = searchRoot(i++, -Constants::Infinity, Constants::Infinity, move, board);
        toMake = move;

        while(i<100 && Timer.Stop().ElapsedMilliseconds() < ThinkTime)
        {

            //aspiration search
            temp = searchRoot(i, val - AspirationValue, val + AspirationValue, move, board);

            if (temp <= val - AspirationValue || temp >= val + AspirationValue)
                temp = searchRoot(i, -Constants::Infinity, Constants::Infinity, move, board);
            val = temp;

            toMake = move;

            i++;
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

    INLINE int Search::searchRoot(int depth, int alpha, int beta, Move& moveToMake, Board& board)
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
                return max;

            board.MakeMove(moves[i]);
            score = -Search::search(depth-1, alpha, beta, board);
            board.UndoMove(moves[i]);

            if( score > max )
            {
                max = score;
                move = i;
            }
        }

        moveToMake = moves[move];
        return max;
    }

}

#endif // SEARCH_H

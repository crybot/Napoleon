#ifndef SEARCH_H
#define SEARCH_H
#include "defines.h"
#include "board.h"
#include "move.h"
#include "constants.h"
#include "movegenerator.h"
#include "constants.h"

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
        static void setScores(Move[], Board&, int, int);
        static void pickMove(Move[], int, int);
        static void orderCaptures(Move[], Board&, int, int);
        static const int R = 2;
        static int moveScores[Constants::MaxMoves];
    };

    inline void Search::IterativeSearch(Board& board)
    {
        Move move;
        Move best;
        Move pv[Constants::MaxPly];
        float branch = 0;
        for (int i=1; i<=7; i++)
        {
            move = searchRoot(i, -Infinity, Infinity, board);
//            pv[0] = move;

//            board.MakeMove(move);
//            std::cout << move.ToAlgebraic() << " ";

//            int l;
//            for (l=1; l<i; l++)
//            {
//                best = board.Table[board.zobrist % board.Table.Size].BestMove;

//                if (best.IsNull())
//                    break;

//                pv[l] = best;
//                board.MakeMove(best);

//                std::cout << best.ToAlgebraic() << " ";
//            }

//            for (int k=l-1; k>=0; k--)
//            {
//                board.UndoMove(pv[k]);
//            }

//            std::cout << std::endl;
//            std::cout << "Branching Factor: " << board.Nps / branch << std::endl;
//            branch = board.Nps;
//            board.Nps = 0;
        }

        board.MakeMove(move);
        board.Display();
        std::cout << "I.A. Move: " << move.ToAlgebraic() << std::endl;
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

#include "search.h"
#include "board.h"
#include "movegenerator.h"
#include "constants.h"
#include "evaluation.h"

namespace Napoleon
{
    int Search::negaMax(int depth, int alpha, int beta, Board &board)
    {
        if( depth == 0 )
            return quiesce(alpha, beta, board);

        bool bSearchPv = true;
        int pos = 0;
        int score;
        Move moves[Constants::MaxMoves + 2];

        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        if (attackers)
            MoveGenerator::GetEvadeMoves(board, attackers, moves, pos);
        else
            MoveGenerator::GetAllMoves(moves, pos, board);

        for (int i=0; i<pos; i++)
        {
            BitBoard pinned = board.GetPinnedPieces();
            if (board.IsMoveLegal(moves[i], pinned))
            {

                board.MakeMove(moves[i]);
                if ( bSearchPv )
                {
                    score = -negaMax( depth - 1, -beta, -alpha, board);
                }
                else
                {
                    score = -negaMax( depth - 1, -alpha-1, -alpha, board);
                    if ( score > alpha ) // in fail-soft ... && score < beta ) is common
                        score = -negaMax( depth - 1, -beta, -alpha, board); // re-search
                }
                board.UndoMove(moves[i]);
                if( score >= beta )
                    return beta;   // fail-hard beta-cutoff
                if( score > alpha )
                {
                    alpha = score; // alpha acts like max in MiniMax
                    bSearchPv = false;  // *1)
                }
            }
        }
        return alpha; // fail-hard
    }

    int Search::quiesce(int alpha, int beta, Board& board)
    {
        int stand_pat = Evaluation::Evaluate(board);
        if( stand_pat >= beta )
            return beta;
        if( alpha < stand_pat )
            alpha = stand_pat;

        int pos = 0;
        int score;
        Move moves[Constants::MaxMoves + 2];

        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        if (attackers)
            MoveGenerator::GetEvadeMoves(board, attackers, moves, pos);
        else
            MoveGenerator::GetCaptures(moves, pos, board);


        for(int i=0; i<pos; i++)
        {
            BitBoard pinned = board.GetPinnedPieces();
            if (board.IsMoveLegal(moves[i], pinned))
            {
                board.MakeMove(moves[i]);
                score = -quiesce( -beta, -alpha, board);
                board.UndoMove(moves[i]);

                if( score >= beta )
                    return beta;
                if( score > alpha )
                    alpha = score;
            }
        }
        return alpha;
    }
}

#include "search.h"
#include "board.h"
#include "movegenerator.h"
#include "constants.h"
#include "evaluation.h"

namespace Napoleon
{
    int Search::search(int depth, int alpha, int beta, Board &board)
    {
        int bound = Alpha;
        int score;

        if((score = board.Table.Probe(board.zobrist, depth, alpha, beta)) != TranspositionTable::Unknown)
            return score;

        if( depth == 0 )
        {
            score = quiesce(alpha, beta, board);
            board.Table.Save(HashEntry(board.zobrist, depth, score, 0, Exact));
            return score;
        }

        int pos = 0;
        Move moves[Constants::MaxMoves + 2];

        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        if (attackers)
            MoveGenerator::GetEvadeMoves(board, attackers, moves, pos);
        else
            MoveGenerator::GetAllMoves(moves, pos, board);

        BitBoard pinned = board.GetPinnedPieces();

        for (int i=0; i<pos; i++)
        {
            if (board.IsMoveLegal(moves[i], pinned))
            {
                board.MakeMove(moves[i]);
                score = -search(depth - 1, -beta, -alpha, board);
                board.UndoMove(moves[i]);

                if( score >= beta )
                {
                    board.Table.Save(HashEntry(board.zobrist, depth, beta, 0, Beta));
                    return beta;   //  fail hard beta-cutoff
                }
                if( score > alpha )
                {
                    bound = Exact;
                    alpha = score; // alpha acts like max in MiniMax
                }
            }
        }

        board.Table.Save(HashEntry(board.zobrist, depth, alpha, 0, bound));
        return alpha;
    }



    // just copied to see improvements on play (it will be reimplemented soon)
    int Search::quiesce(int alpha, int beta, Board& board)
    {
        int stand_pat = Evaluation::Evaluate(board);
        if( stand_pat >= beta )
            return beta;
        if( alpha < stand_pat )
            alpha = stand_pat;

        int pos = 0;
        int score;
        Move moves[Constants::MaxMoves];

        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        if (attackers)
            MoveGenerator::GetEvadeMoves(board, attackers, moves, pos);
        else
            MoveGenerator::GetCaptures(moves, pos, board);

        BitBoard pinned = board.GetPinnedPieces();
        for(int i=0; i<pos; i++)
        {

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

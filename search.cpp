#include "search.h"
#include "board.h"
#include "movegenerator.h"
#include "evaluation.h"
#include <cassert>
#include <cstring>

namespace Napoleon
{
    StopWatch Search::Timer;
    int Search::ThinkTime;
    int Search::moveScores[Constants::MaxMoves];
    int Search::history[2][64][64] = {{{1}}};
    Move Search::killerMoves[Constants::MaxPly][2];

    int Search::search(int depth, int alpha, int beta, Board& board)
    {
        board.Nps++;

        int bound = Alpha;
        int pos = 0;
        int score;
        int legal = 0;
        Move best = Constants::NullMove;
        Move moves[Constants::MaxMoves];

        if((score = board.Table.Probe(board.zobrist, depth, alpha, &best, beta)) != TranspositionTable::Unknown)
            return score;

        if(depth == 0)
            return quiesce(alpha, beta, board);

        BitBoard pinned = board.GetPinnedPieces();

        // hash move
        if(!best.IsNull())
        {
            assert(board.IsMoveLegal(best, pinned));

            legal++;
            board.MakeMove(best);
            score = -search(depth - 1, -beta, -alpha, board);
            board.UndoMove(best);

            if(score >= beta)
                return beta;

            if(score > alpha)
            {
                bound = Exact;
                alpha = score;
            }
        }

        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        // null move pruning search
        if(board.AllowNullMove && depth >= 3 && !attackers)
        {
            int R = depth > 5 ? 3 : 2; // dynamic depth-based reduction
            board.MakeNullMove();
            score = -search(depth - R - 1 , -beta, -beta+1, board);
            board.UndoNullMove();

            if(score >= beta)
                return beta;
        }

        // internal iterative deepening
        if (depth >= 3 && best.IsNull())
        {
            int R = depth > 5 ? 4 : 2;

            search(depth/R - 1, -Infinity, Infinity, board);

            if((score = board.Table.Probe(board.zobrist, depth, alpha, &best, beta)) != TranspositionTable::Unknown)
                return score;

            if (!best.IsNull())
            {
                assert(board.IsMoveLegal(best, pinned));

                legal++;
                board.MakeMove(best);
                score = -search(depth - 1, -beta, -alpha, board);
                board.UndoMove(best);

                if( score >= beta )
                    return beta;

                if( score > alpha )
                {
                    bound = Exact;
                    alpha = score;
                }
            }
        }

        MoveGenerator::GetPseudoLegalMoves<false>(moves, pos, attackers, board); // get captures and non-captures
        setScores(moves, board, depth, pos); // move-list, board, actual depth, number of moves

        for(int i=0; i<pos; i++)
        {
            pickMove(moves, i, pos);
            if(board.IsMoveLegal(moves[i], pinned))
            {
                legal++;
                board.MakeMove(moves[i]);
                score = -search(depth - 1, -beta, -alpha, board);
                board.UndoMove(moves[i]);

                if( score >= beta )
                {
                    //killer moves and history heuristic
                    if(!moves[i].IsCapture())
                    {
                        if (moves[i] != killerMoves[depth][0])
                        {
                            killerMoves[depth][1] = killerMoves[depth][0];
                        }
                        killerMoves[depth][0] = moves[i];
                        history[board.SideToMove][moves[i].FromSquare][moves[i].ToSquare]++; // seems faster than += depth^2
                    }

                    board.Table.Save(board.zobrist, depth, beta, best, Beta);
                    return beta;   //  fail hard beta-cutoff
                }
                if(score > alpha)
                {
                    bound = Exact;
                    alpha = score; // alpha acts like max in MiniMax
                    best = moves[i];
                }
            }
        }

        // check for stalemate and checkmate
        if (legal == 0)
        {
            if (board.IsCheck)
            {
                board.Table.Save(board.zobrist, depth, -16384 - depth*depth, best, Exact);
                return -16384 - depth*depth;
            }
            else
            {
                board.Table.Save(board.zobrist, depth, 0, best, Exact);
                return 0;
            }
        }

        // check for fifty moves rule
        if (board.HalfMoveClock >= 100)
        {
            board.Table.Save(board.zobrist, depth, 0, best, Exact);
            return 0;
        }

        board.Table.Save(board.zobrist, depth, alpha, best, bound);
        return alpha;
    }


    int Search::quiesce(int alpha, int beta, Board& board)
    {
        board.Nps++;
        int stand_pat = Evaluation::Evaluate(board);
        if( stand_pat >= beta )
            return beta;

        int Delta = Constants::Piece::PieceValue[PieceType::Queen];

        // big delta futility pruning
        if ( stand_pat < alpha - Delta )
            return alpha;

        if( alpha < stand_pat )
            alpha = stand_pat;

        int pos = 0;
        int score;
        Move moves[Constants::MaxMoves];

        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        MoveGenerator::GetPseudoLegalMoves<true>(moves, pos, attackers, board); // get only captures

        BitBoard pinned = board.GetPinnedPieces();
        for(int i=0; i<pos; i++)
        {
            orderCaptures(moves, board, i, pos);
            if (board.IsMoveLegal(moves[i], pinned))
            {
                // delta futility pruning
                if (Constants::Piece::PieceValue[moves[i].PieceCaptured] + stand_pat +  200 < alpha)
                    continue;

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

    /// set scores for sorting moves
    /// 1) winning captures
    /// 2) equal captures
    /// 3) killer moves
    /// 4) losing captures
    /// 5) all other moves in history heuristic order
    void Search::setScores(Move moves[], Board& board, int depth, int high)
    {
        using namespace Constants::Piece;

        int min = 0;
        int max = 0;
        int captureScore;
        int historyScore;

        for (int i=0; i<high; i++)
        {
            // MVV-LVA
            if (moves[i].IsCapture())
            {
                moveScores[i] = captureScore = PieceValue[moves[i].PieceCaptured] - PieceValue[board.PieceSet[moves[i].FromSquare].Type];
                if (captureScore < min)
                    min = captureScore;
            }
            else if (moves[i] == killerMoves[depth][0])
                moveScores[i] = - 1 ;
            else if (moves[i] == killerMoves[depth][1])
                moveScores[i] = - 2 ;
            else if ((historyScore = history[board.SideToMove][moves[i].FromSquare][moves[i].ToSquare]) > max)
                max = historyScore;
        }

        for (int i=0; i<high; i++)
        {
            if (!moves[i].IsCapture() && moves[i] != killerMoves[depth][0] && moves[i] != killerMoves[depth][1])
                moveScores[i] = history[board.SideToMove][moves[i].FromSquare][moves[i].ToSquare] - max + min - 3;
        }
    }

    void Search::pickMove(Move moves[], int low, int high)
    {
        int max = low;

        for (int i=low+1; i<high; i++)
            if (moveScores[i] > moveScores[max])
                max = i;

        if (max != low)
        {
            Move temp = moves[low];
            moves[low] = moves[max];
            moves[max] = temp;

            int tempScore = moveScores[low];
            moveScores[low] = moveScores[max];
            moveScores[max] = tempScore;
        }
    }

    void Search::orderCaptures(Move moves[], Board& board, int low, int high)
    {
        /// MVV-LVA
        /// (Most Valuable Victim - Least Valuable Aggressor)

        using namespace Constants::Piece;

        int min = low;

        for (int i=low+1; i<high; i++)
        {
            if (moves[i].IsCapture())
            {
                if (PieceValue[moves[i].PieceCaptured] > PieceValue[moves[min].PieceCaptured])
                {
                    if (min != low)
                    {
                        if (PieceValue[board.PieceSet[moves[i].FromSquare].Type] < PieceValue[board.PieceSet[moves[min].FromSquare].Type])
                            min = i;
                    }
                    else
                        min = i;
                }
            }
        }

        if (min != low)
        {
            Move temp = moves[low];
            moves[low] = moves[min];
            moves[min] = temp;
        }
    }


}

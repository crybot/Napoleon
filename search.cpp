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
    int Search::history[2][64][64];
    int Search::lastScore;
    Move Search::killerMoves[Constants::MaxPly][2];

    // iterative deepening
    void Search::IterativeSearch(Board& board)
    {
        Move move;
        Move best;
        Move toMake;
        Move pv[Constants::MaxPly];
        const int AspirationValue = 50;

        memset(history, 0, sizeof(history));
        memset(board.Table.Table, 0, sizeof(HashEntry) * board.Table.Size);

        Timer.Start();
        ThinkTime = 1000;

        int val;
        int temp;
        int i = 8;

        val = searchRoot(i++, -Constants::Infinity, Constants::Infinity, move, board);

        std::cout << move.ToAlgebraic() << std::endl;

        while(i<=9)
        {
            //            aspiration search
            temp = searchRoot(i, val - AspirationValue, val + AspirationValue, move, board);

            if (temp <= val - AspirationValue || temp >= val + AspirationValue)
                temp = searchRoot(i, -Constants::Infinity, Constants::Infinity, move, board);
            val = temp;

            toMake = move;

            i++;
            pv[0] = toMake;

            board.MakeMove(toMake);
            std::cout << toMake.ToAlgebraic() << " ";

            int l;
            for (l=1; l<i; l++)
            {
                best = board.Table[board.zobrist % board.Table.Size].BestMove;

                if (best.IsNull())
                    break;

                pv[l] = best;

                assert(!best.IsNull());

                board.MakeMove(best);

                std::cout << best.ToAlgebraic() << " ";
            }

            for (int k=l-1; k>=0; k--)
            {
                assert(!pv[k].IsNull());

                board.UndoMove(pv[k]);
            }

            std::cout << std::endl;
        }

        std::cout << "A.I. Move: " << toMake.ToAlgebraic() << std::endl;

        //        int pos = 0;
        //        Move moves[Constants::MaxMoves];

        //        MoveGenerator::GetLegalMoves(moves, pos, board);

        board.MakeMove(toMake);
        board.Display();

        //       std::cout << "Cutoff on first move: " << board.FirstMoveCutoff << std::endl;
        //       std::cout << "Total Cutoffs: " << board.TotalCutoffs << std::endl;
        //       std::cout <<  Console::Green << "First Move Cutoff percentage: " << ((float)board.FirstMoveCutoff / (float)board.TotalCutoffs) * 100 << "%" << std::endl;
        //       std::cout <<  Console::Reset << std::endl;
        //       board.FirstMoveCutoff = 0;
        //       board.TotalCutoffs = 0;

    }

    int Search::searchRoot(int depth, int alpha, int beta, Move& moveToMake, Board& board)
    {
        int max = alpha;
        int pos = 0;
        int move = 0;
        int score;
        Move moves[Constants::MaxMoves];

        MoveGenerator::GetLegalMoves(moves, pos, board);

        for (int i=0; i<pos; i++)
        {
            //            if (Timer.Stop().ElapsedMilliseconds() >= ThinkTime)
            //                return -Constants::Infinity;

            board.MakeMove(moves[i]);

            if (i==0)
                score = -Search::search<Constants::Node::Pv>(depth-1, alpha, beta, board);
            else
                score = -Search::search<Constants::Node::All>(depth-1, alpha, beta, board);

            board.UndoMove(moves[i]);

            if( score > max )
            {
                max = score;
                move = i;
            }

            std::cout << moves[i].ToAlgebraic() <<  " " << score << std::endl;
        }

        moveToMake = moves[move];
        return max;
    }


    template<int NodeType>
    int Search::search(int depth, int alpha, int beta, Board& board)
    {
        using namespace Constants::Node;
        board.Nps++;

        const bool isPv = NodeType == Pv;
        const bool isCut = NodeType == Cut;
        const bool isAll = NodeType == All;

        int bound = Alpha;
        int pos = 0;
        int score;
        int legal = 0;
        Move best = Constants::NullMove;
        Move moves[Constants::MaxMoves];

        // Transposition table lookup
        if((score = board.Table.Probe(board.zobrist, depth, alpha, &best, beta)) != TranspositionTable::Unknown)
            return score;

        if(depth == 0)
            return quiescence(alpha, beta, board);

        BitBoard pinned = board.GetPinnedPieces();

        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        // adaptive null move pruning
        if(board.AllowNullMove && depth >= 3 && !attackers)
        {
            int R = depth > 5 ? 3 : 2; // dynamic depth-based reduction

            board.MakeNullMove();
            score = -search<isPv ? Pv : All>(depth - R - 1 , -beta, -beta+1, board); // make a null-window search (we don't care by how much it fails high, if it does)
            board.UndoNullMove();

            if(score >= beta)
                return beta;
        }

        // internal iterative deepening (IID)
        if (depth >= 3 && best.IsNull())
        {
            int R = 2;

            search<isPv ? Pv : All>(depth - R - 1, -Constants::Infinity, Constants::Infinity, board); // make a full width search to find a new bestmove

            //Transposition table lookup
            if((score = board.Table.Probe(board.zobrist, depth, alpha, &best, beta)) != TranspositionTable::Unknown)
                return score;
        }

        // make best move
        if(!best.IsNull())
        {
            assert(board.IsMoveLegal(best, pinned));

            legal++;
            board.MakeMove(best);
            score = -search<isPv ? Pv : All>(depth - 1, -beta, -alpha, board);
            board.UndoMove(best);

            if(score >= beta)
            {
                board.FirstMoveCutoff++; // DEBUG
                board.TotalCutoffs++; // DEBUG
                return beta;
            }

            if(score > alpha)
            {
                bound = Exact;
                alpha = score;
            }
        }

        MoveGenerator::GetPseudoLegalMoves<false>(moves, pos, attackers, board); // get captures and non-captures
        setScores(moves, board, depth, pos); // move-list, board, actual depth, number of moves

        bool PVS = true;

        for(int i=0; i<pos; i++)
        {
            pickMove(moves, i, pos);
            if(board.IsMoveLegal(moves[i], pinned))
            {
                legal++;
                board.MakeMove(moves[i]);

                if (PVS)
                {
                    score = -search<isPv ? Pv : All>(depth-1, -beta, -alpha, board);
                }
                else
                {
                    score = -search<All>(depth-1, -alpha-1, -alpha, board);
                    if (score > alpha && score < beta)
                        score = -search<All>(depth-1, -beta, -alpha, board);
                }

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
                        history[board.SideToMove][moves[i].FromSquare][moves[i].ToSquare] ++; // seems faster than += depth^2
                    }

                    board.Table.Save(board.zobrist, depth, beta, best, Beta);

                    if (i == 0) // DEBUG
                        board.FirstMoveCutoff++; // DEBUG

                    board.TotalCutoffs++; // DEBUG

                    return beta;   //  fail hard beta-cutoff
                }
                if(score > alpha)
                {
                    PVS = false;
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


    int Search::quiescence(int alpha, int beta, Board& board)
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
                score = -quiescence( -beta, -alpha, board);
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
    ///
    /// Every history move has a positive score, so to order them such that they
    /// are always after losing captures (which have a score <= 0) we found
    /// the minimum score of the captures and the maximum score of the history moves.
    /// Then we assing to each history move a score calculated with this formula:
    /// Score = HistoryScore - HistoryMax + CapturesMin - 3
    /// The - 3 factor handle the situation where there are no losing captures,
    /// but history moves should still stay after killer moves
    /// (which have score -1 and -2). Without that, the best history move
    /// would score 0 and would be analyzed before killer moves.

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

    // sort only captures (only used in quiescence search)
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

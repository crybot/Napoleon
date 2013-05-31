#include "search.h"
#include "board.h"
#include "movegenerator.h"
#include "evaluation.h"
#include "uci.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace Napoleon
{
    const int Search::AspirationValue = 40;
    SearchTask Search::Task = Stop;
    StopWatch Search::Timer;
    int Search::Time[2] = { 60000, 60000 };
    int Search::ThinkTime;
    int Search::moveScores[Constants::MaxMoves];
    int Search::history[2][4096];
    int Search::nodes;
    Move Search::killerMoves[Constants::MaxPly][2];

    // external interface to the client.
    // it sends the move to the uci gui
    void Search::StartThinking(Board& board)
    {
        int time = Time[board.SideToMove];
        ThinkTime = time / (30 - (time/(60*1000)));
        Task = Think;
        Move move = iterativeSearch(board);
        Uci::SendCommand<Command::Move>(move.ToAlgebraic());
    }

    void Search::InfiniteSearch(Board& board)
    {
        Task = Infinite;
        Move move = iterativeSearch(board);
        Uci::SendCommand<Command::Move>(move.ToAlgebraic());
    }

    void Search::StopThinking()
    {
        Task = Stop;
    }

    // iterative deepening
    Move Search::iterativeSearch(Board& board)
    {
        Move move;
        Move toMake = Constants::NullMove;
        int lastTime = 0;
        int val;
        int temp;
        int i = 1;

        memset(history, 0, sizeof(history));

        Timer.Start();
        nodes = 0;

        val = searchRoot(i, -Constants::Infinity, Constants::Infinity, move, board);

        Uci::SendCommand<Command::Info>(GetInfo(board, move, val, i++, lastTime));
        lastTime = Timer.Stop().ElapsedMilliseconds();

        while((i<100 && Timer.Stop().ElapsedMilliseconds() < ThinkTime && Timer.Stop().ElapsedMilliseconds() / ThinkTime < 0.50) || Task == Infinite )
        {
            if (Task == Stop)
                break;

            nodes = 0;
            //            aspiration search
            temp = searchRoot(i, val - AspirationValue, val + AspirationValue, move, board);

            if (temp <= val - AspirationValue || temp >= val + AspirationValue)
                temp = searchRoot(i, -Constants::Infinity, Constants::Infinity, move, board);
            val = temp;

            if (val != -Constants::Unknown)
                toMake = move;

            Uci::SendCommand<Command::Info>(GetInfo(board, toMake, val, i++, lastTime));
            lastTime = Timer.Stop().ElapsedMilliseconds();
        }

        StopThinking();

        return toMake;

        //       std::cout << "Cutoff on first move: " << board.FirstMoveCutoff << std::endl;
        //       std::cout << "Total Cutoffs: " << board.TotalCutoffs << std::endl;
        //       std::cout <<  Console::Green << "First Move Cutoff percentage: " << ((float)board.FirstMoveCutoff / (float)board.TotalCutoffs) * 100 << "%" << std::endl;
        //       std::cout <<  Console::Reset << std::endl;
        //       board.FirstMoveCutoff = 0;
        //       board.TotalCutoffs = 0;

    }

    int Search::searchRoot(int depth, int alpha, int beta, Move& moveToMake, Board& board)
    {
        int pos = 0;
        int move = 0;
        int score;
        Move moves[Constants::MaxMoves];

        MoveGenerator::GetLegalMoves(moves, pos, board);

        for (int i=0; i<pos; i++)
        {
            if ((Timer.Stop().ElapsedMilliseconds() >= ThinkTime || Timer.Stop().ElapsedMilliseconds()/ThinkTime >= 0.60 || Task == Stop) && Task != Infinite)
                return -Constants::Unknown;

            board.MakeMove(moves[i]);
            score = -Search::search(depth-1, -beta, -alpha, board);
            board.UndoMove(moves[i]);

            if (score > alpha)
            {
                move = i;
                if (score >= beta)
                {
                    moveToMake = moves[i];
                    return beta;
                }

                alpha = score;
            }

            //            std::cout << moves[i].ToAlgebraic() <<  " " << score << std::endl;
        }

        moveToMake = moves[move];
        return alpha;
    }

    int Search::search(int depth, int alpha, int beta, Board& board)
    {
        nodes++;

        bool futility = false;
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

        // razoring
        if (depth == 4 && !attackers && board.Material[board.SideToMove] > 4000 && best.IsNull())
        {
            score = Evaluation::Evaluate(board);

            if (score  + (100*(depth-1)) < alpha)
                return score;
        }

        // adaptive null move pruning

        if(board.AllowNullMove && depth >= 3 && !attackers && board.Material[board.SideToMove] > 4000)
        {
            int R = depth > 5 ? 3 : 2; // dynamic depth-based reduction

            board.MakeNullMove();
            score = -search(depth - R - 1 , -beta, -beta+1, board); // make a null-window search (we don't care by how much it fails high, if it does)
            board.UndoNullMove();

            if(score >= beta)
                return beta;
        }

        // internal iterative deepening (IID)
        if (depth >= 3 &&  best.IsNull())
        {
            int R = 2;

            search(depth - R - 1, Constants::Infinity, -Constants::Infinity, board); // make a full width search to find a new bestmove

            //Transposition table lookup
            if((score = board.Table.Probe(board.zobrist, depth, alpha, &best, beta)) != TranspositionTable::Unknown)
                return score;
        }

        // make best move
        if(!best.IsNull())
        {
            legal++;
            board.MakeMove(best);
            score = -search(depth - 1, -beta, -alpha, board);
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

        // extended futility pruning
        int margin[] = { 0, Constants::Piece::PieceValue[PieceType::Pawn],  Constants::Piece::PieceValue[PieceType::Rook] };

        if (!attackers && std::abs(alpha) < Constants::Infinity/2 && depth <=2 && Evaluation::Evaluate(board) + margin[depth] <= alpha)
            futility = true;

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

                if (futility
                        && !board.IsCapture(moves[i])
                        && !moves[i].IsPromotion()
                        && !board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove)
                        )
                {
                    board.UndoMove(moves[i]);
                    continue;
                }

                if (PVS)
                {
                    score = -search(depth-1, -beta, -alpha, board);
                }
                else
                {
                    score = -search(depth-1, -alpha-1, -alpha, board);
                    if (score > alpha/* && score < beta*/)
                        score = -search(depth-1, -beta, -alpha, board);
                }

                board.UndoMove(moves[i]);

                if( score >= beta )
                {
                    //killer moves and history heuristic
                    if(!board.IsCapture(moves[i]))
                    {
                        if (moves[i] != killerMoves[depth][0])
                        {
                            killerMoves[depth][1] = killerMoves[depth][0];
                        }
                        killerMoves[depth][0] = moves[i];
                        history[board.SideToMove][moves[i].ButterflyIndex()] += depth*depth;
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
                alpha = -Constants::Infinity - depth;
            else
                alpha = 0;
        }

        // check for fifty moves rule
        if (board.HalfMoveClock >= 100)
            alpha = 0;

        board.Table.Save(board.zobrist, depth, alpha, best, bound);
        return alpha;
    }


    int Search::quiescence(int alpha, int beta, Board& board)
    {
        nodes++;
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
                if (Constants::Piece::PieceValue[board.PieceSet[moves[i].ToSquare()].Type] + stand_pat + 200 < alpha && !moves[i].IsPromotion())
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
        int captured;

        for (int i=0; i<high; i++)
        {
            captured = board.PieceSet[moves[i].ToSquare()].Type;
            // MVV-LVA
            if (board.IsCapture(moves[i]))
            {
                moveScores[i] = captureScore = PieceValue[captured] - PieceValue[board.PieceSet[moves[i].FromSquare()].Type];
                if (captureScore < min)
                    min = captureScore;
            }
            else if (moves[i] == killerMoves[depth][0])
                moveScores[i] = - 1 ;
            else if (moves[i] == killerMoves[depth][1])
                moveScores[i] = - 2 ;
            else if ((historyScore = history[board.SideToMove][moves[i].ButterflyIndex()]) > max)
                max = historyScore;
        }

        for (int i=0; i<high; i++)
        {
            if (!board.IsCapture(moves[i]) && moves[i] != killerMoves[depth][0] && moves[i] != killerMoves[depth][1])
                moveScores[i] = history[board.SideToMove][moves[i].ButterflyIndex()] - max + min - 3;
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
        int from;
        int to;
        Byte captured;

        for (int i=low+1; i<high; i++)
        {
            if (board.IsCapture(moves[i]))
            {
                from = moves[i].FromSquare();
                to = moves[i].ToSquare();
                captured = board.PieceSet[to].Type;

                if (PieceValue[captured] >  PieceValue[board.PieceSet[moves[min].ToSquare()].Type])
                {
                    if (min != low)
                    {
                        if (PieceValue[board.PieceSet[from].Type] < PieceValue[board.PieceSet[moves[min].FromSquare()].Type])
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


    std::string Search::GetInfo(Board& board, Move toMake, int score, int depth, int lastTime)
    {
        std::ostringstream info;
        std::string PV;
        Move best;
        Move pv[Constants::MaxMoves];

        pv[0] = toMake;

        board.MakeMove(toMake);

        PV += toMake.ToAlgebraic() + " ";

        int l;
        for (l=1; l<depth; l++)
        {
            best = board.Table.GetPv(board.zobrist);

            if (best.IsNull())
            {
                break;
            }

            pv[l] = best;

            assert(!best.IsNull());

            board.MakeMove(best);

            PV += best.ToAlgebraic() + " ";
        }

        for (int k=l-1; k>=0; k--)
        {
            assert(k<depth);
            assert(!pv[k].IsNull());

            board.UndoMove(pv[k]);
        }

        double delta = Timer.Stop().ElapsedMilliseconds() - lastTime;
        double nps = delta > 0 ? nodes / delta : nodes / 1;
        nps*=1000;

        info << "depth " << depth << " score cp " << score << " time " << Timer.Stop().ElapsedMilliseconds() << " nodes "
             << nodes << " nps " << int(nps) << " pv " << PV;

        return info.str();
    }
}

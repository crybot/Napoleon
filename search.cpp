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
    const int Search::AspirationValue = 50;
    bool Search::MoveTime;
    SearchTask Search::Task = Stop;
    StopWatch Search::Timer;
    int Search::Time[2] = { 60000, 60000 };
    int Search::ThinkTime;
    int Search::moveScores[Constants::MaxMoves];
    int Search::history[2][64*64];
    int Search::nodes;
    Move Search::killerMoves[Constants::MaxPly][2];

    // direct interface to the client.
    // it sends the move to the uci gui
    void Search::StartThinking(Board& board)
    {
        int time = Time[board.SideToMove()];

        // if we have an exact movetime we use that value, else we use
        // a fraction of the side to move remaining time
        ThinkTime = MoveTime ? ThinkTime : (time / (30 - (time/(60*1000))));

        if (Task != Infinite) Task = Think;

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
        int score;
        int temp;
        int i = 1;

        memset(history, 0, sizeof(history));

        Timer.Restart();
        nodes = 0;

        score = searchRoot(i++, -Constants::Infinity, Constants::Infinity, move, board);

        while((i<100 && Timer.ElapsedMilliseconds() < ThinkTime && Timer.ElapsedMilliseconds() / ThinkTime < 0.50) || Task == Infinite )
        {
            if (Task == Stop)
                break;

            nodes = 0;
            //            aspiration search
            temp = searchRoot(i, score - AspirationValue, score + AspirationValue, move, board);

            if (temp <= score - AspirationValue || temp >= score + AspirationValue)
                temp = searchRoot(i, -Constants::Infinity, Constants::Infinity, move, board);
            score = temp;

            if (score != -Constants::Unknown)
                toMake = move;

            i++;
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
        int startTime = Timer.ElapsedMilliseconds();
        Move moves[Constants::MaxMoves];

        MoveGenerator::GetLegalMoves(moves, pos, board);

        for (int i=0; i<pos; i++)
        {
            if ((Timer.ElapsedMilliseconds() >= ThinkTime || Timer.ElapsedMilliseconds()/ThinkTime >= 0.65 || Task == Stop) && Task != Infinite)
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
                    Uci::SendCommand<Command::Info>(GetInfo(board, moveToMake, beta, depth, startTime)); // sends info to the gui
                    return beta;
                }

                alpha = score;
            }
        }

        moveToMake = moves[move];
        Uci::SendCommand<Command::Info>(GetInfo(board, moveToMake, alpha, depth, startTime)); // sends info to the gui

        return alpha;
    }

    int Search::search(int depth, int alpha, int beta, Board& board)
    {
        nodes++;

        bool futility = false;
        ScoreType bound = ScoreType::Alpha;
        int pos = 0;
        int score;
        int legal = 0;
        Move best = Constants::NullMove;
        Move moves[Constants::MaxMoves];

        if (Task == Stop)
            return alpha;

        // Transposition table lookup
        if((score = board.Table.Probe(board.zobrist, depth, alpha, &best, beta)) != TranspositionTable::Unknown)
            return score;

        // call to quiescence search
        if(depth == 0)
            return quiescence(alpha, beta, board);

        BitBoard pinned = board.GetPinnedPieces();

        BitBoard attackers = board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove());

        // enhanced deep razoring
        if (depth < 4
                && !attackers
                && board.Material(board.SideToMove()) > Constants::Eval::MiddleGameMat
                && best.IsNull()
                && !board.IsPromotingPawn())
        {
            score = Evaluation::Evaluate(board);

            int margin = razorMargin(depth);

            if (score + margin <= alpha)
            {
                int s = quiescence(alpha-margin, beta-margin, board);
                if (s <= alpha - margin)
                    return s;
            }
        }

        // adaptive null move pruning
        if(board.AllowNullMove()
                && depth >= 3
                && !attackers
                && board.Material(board.SideToMove()) > Constants::Eval::MiddleGameMat)
        {
            int R = depth > 5 ? 3 : 2; // dynamic depth-based reduction

            board.MakeNullMove();
            score = -search(depth - R - 1 , -beta, -beta+1, board); // make a null-window search (we don't care by how much it fails high, if it does)
            board.UndoNullMove();

            if(score >= beta)
                return beta;
        }

        // internal iterative deepening (IID)
        if (depth > 4 && best.IsNull())
        {
            int R = 2;

            search(depth - R - 1, -Constants::Infinity, Constants::Infinity, board); // make a full width search to find a new bestmove

            //Transposition table lookup
            if((score = board.Table.Probe(board.zobrist, depth, alpha, &best, beta)) != TranspositionTable::Unknown)
                return score;
        }

        // make best move (hash move)
        if(!best.IsNull())
        {
            assert(board.IsMoveLegal(best, pinned));
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
                bound = ScoreType::Exact;
                alpha = score;
            }
        }

        if (board.IsRepetition())
            return 0;

        // extended futility pruning condition
        if (!attackers
                && depth <=2
                && std::abs(alpha) < Constants::Infinity-100
                && Evaluation::Evaluate(board) + futilityMargin(depth) <= alpha)
        {
            futility = true;
        }


        MoveGenerator::GetPseudoLegalMoves<false>(moves, pos, attackers, board); // get captures and non-captures
        setScores(moves, board, depth, pos); // set moves score used by 'pickMove' for picking the best untried move

        // principal variation search
        bool PVS = true;
        bool capture;

        for(int i=0; i<pos; i++)
        {
            pickMove(moves, i, pos);

            if(board.IsMoveLegal(moves[i], pinned))
            {
                legal++;
                capture = board.IsCapture(moves[i]);
                board.MakeMove(moves[i]);

                // extended futility pruning application
                if (futility
                        && i > 0
                        && !capture
                        && !moves[i].IsPromotion()
                        && !board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove())
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
                        history[board.SideToMove()][moves[i].ButterflyIndex()] += depth*depth;
                    }

                    board.Table.Save(board.zobrist, depth, beta, best, ScoreType::Beta);

                    if (i == 0) // DEBUG
                        board.FirstMoveCutoff++; // DEBUG

                    board.TotalCutoffs++; // DEBUG

                    return beta;   //  fail hard beta-cutoff
                }
                if(score > alpha)
                {
                    PVS = false;
                    bound = ScoreType::Exact;
                    alpha = score; // alpha acts like max in MiniMax
                    best = moves[i];
                }
            }
        }

        // check for stalemate and checkmate
        if (legal == 0)
        {
            if (board.IsCheck())
                alpha = -Constants::Infinity + depth; // return best score (for the minimazer) for the deepest mate
            else
                alpha = 0; // return draw score (TODO contempt factor)
        }

        // check for fifty moves rule
        if (board.HalfMoveClock() >= 100)
            alpha = 0;

        board.Table.Save(board.zobrist, depth, alpha, best, bound);
        return alpha;
    }

    // quiescence is called at horizon nodes (depth = 0)
    int Search::quiescence(int alpha, int beta, Board& board)
    {
        nodes++;
        int stand_pat = Evaluation::Evaluate(board);
        if( stand_pat >= beta )
            return beta;

        int Delta = Constants::Piece::PieceValue[PieceType::Queen];

        if (board.IsPromotingPawn())
            Delta += Constants::Piece::PieceValue[PieceType::Queen] - Constants::Piece::PieceValue[PieceType::Pawn];

        // big delta futility pruning
        if ( stand_pat < alpha - Delta)
            return alpha;

        if( alpha < stand_pat )
            alpha = stand_pat;

        int pos = 0;
        int score;
        Move moves[Constants::MaxMoves];

        BitBoard attackers = board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove());

        if (attackers)
            return search(1, alpha, beta, board);

        MoveGenerator::GetPseudoLegalMoves<true>(moves, pos, attackers, board); // get only captures

        BitBoard pinned = board.GetPinnedPieces();
        for(int i=0; i<pos; i++)
        {
            orderCaptures(moves, board, i, pos);
            if (board.IsMoveLegal(moves[i], pinned))
            {
                // delta futility pruning
                if (Constants::Piece::PieceValue[board.GetPieceOnSquare(moves[i].ToSquare()).Type] + stand_pat + 200 < alpha && !moves[i].IsPromotion())
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
            captured = board.GetPieceOnSquare(moves[i].ToSquare()).Type;
            // MVV-LVA
            if (board.IsCapture(moves[i]))
            {
                moveScores[i] = captureScore = PieceValue[captured] - PieceValue[board.GetPieceOnSquare(moves[i].FromSquare()).Type];
                if (captureScore < min)
                    min = captureScore;
            }
            else if (moves[i] == killerMoves[depth][0])
                moveScores[i] = - 1 ;
            else if (moves[i] == killerMoves[depth][1])
                moveScores[i] = - 2 ;
            else if ((historyScore = history[board.SideToMove()][moves[i].ButterflyIndex()]) > max)
                max = historyScore;
        }

        for (int i=0; i<high; i++)
        {
            if (!board.IsCapture(moves[i]) && moves[i] != killerMoves[depth][0] && moves[i] != killerMoves[depth][1])
                moveScores[i] = history[board.SideToMove()][moves[i].ButterflyIndex()] - max + min - 3;
        }
    }

    // make a selection sort on the move array for picking the best untried move
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
                captured = board.GetPieceOnSquare(to).Type;

                if (PieceValue[captured] >  PieceValue[board.GetPieceOnSquare(moves[min].ToSquare()).Type])
                {
                    if (min != low)
                    {
                        if (PieceValue[board.GetPieceOnSquare(from).Type] < PieceValue[board.GetPieceOnSquare(moves[min].FromSquare()).Type])
                            min = i;
                    }
                    else
                        min = i;
                }
            }
        }

        if (min != low)
        {
            std::swap(moves[low], moves[min]);
        }
    }

    // extract the pv line from transposition table
    std::string Search::GetPv(Board& board, Move toMake, int depth)
    {
        std::string pv;

        if (toMake.IsNull() || depth == 0)
            return pv;
        else
        {
            pv = toMake.ToAlgebraic() + " ";

            board.MakeMove(toMake);
            pv += GetPv(board, board.Table.GetPv(board.zobrist), depth-1);
            board.UndoMove(toMake);

            return pv;
        }
    }

    // return search info
    std::string Search::GetInfo(Board& board, Move toMake, int score, int depth, int lastTime)
    {
        std::ostringstream info;
        double delta = Timer.ElapsedMilliseconds() - lastTime;
        double nps = (delta > 0 ? nodes / delta : nodes / 1)*1000;

        info << "depth " << depth << " score cp " << score << " time " << Timer.ElapsedMilliseconds() << " nodes "
             << nodes << " nps " << static_cast<int>(nps) << " pv " << GetPv(board, toMake, depth);

        return info.str();
    }
}

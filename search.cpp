#include "search.h"
#include "board.h"
#include "movegenerator.h"
#include "evaluation.h"
#include "uci.h"
#include "moveselector.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <algorithm>

namespace Napoleon
{
    const int Search::AspirationValue = 50;
    bool Search::StopSignal = true;
    int Search::GameTime[2];
    int Search::MoveTime;
    bool Search::sendOutput;

    SearchInfo Search::searchInfo;

    // direct interface to the client.
    // it sends the move to the uci gui
    Move Search::StartThinking(SearchType type, Board& board, bool verbose, bool san)
    {
        // NEED to test if it's better to clear the transposition table every time a new search starts.
        // empirical data suggests that it is better.
        board.Table.Clear();

        sendOutput=verbose;
        StopSignal = false;

        if(type == SearchType::Infinite)
        {
            searchInfo.NewSearch(); // default time = Time::Infinite
        }
        else
        {
            int time;

            if (type == SearchType::TimePerGame)
            {
                int gameTime = GameTime[board.SideToMove()];
                time = gameTime / 30 - (gameTime/(60*1000));
            }
            else // TimePerMove
            {
                time = MoveTime;
            }

            searchInfo.NewSearch(time);
        }

        Move move = iterativeSearch(board);

        if (sendOutput)
        {
            if (san)
                Uci::SendCommand<Command::Move>(move.ToSan(board));
            else
                Uci::SendCommand<Command::Move>(move.ToAlgebraic());
        }

        searchInfo.StopSearch();
        return move;
    }

    void Search::StopThinking()
    {
        StopSignal = true;
    }

    // iterative deepening
    Move Search::iterativeSearch(Board& board)
    {
        Move move;
        Move toMake = Constants::NullMove;
        int score;
        int temp;

        score = searchRoot(searchInfo.MaxDepth(), -Constants::Infinity, Constants::Infinity, move, board);
        searchInfo.IncrementDepth();

        while((searchInfo.MaxDepth() < 100 && !searchInfo.TimeOver()))
        {
            if (StopSignal)
                break;

            searchInfo.ResetNodes();

            // aspiration search
            temp = searchRoot(searchInfo.MaxDepth(), score - AspirationValue, score + AspirationValue, move, board);

            if (temp <= score - AspirationValue)
            {
                temp = searchRoot(searchInfo.MaxDepth(), -Constants::Infinity, score + AspirationValue, move, board);
            }
            else if (temp >= score + AspirationValue)
            {
                temp = searchRoot(searchInfo.MaxDepth(), score - AspirationValue, Constants::Infinity, move, board);
            }

            score = temp;

            if (score != Constants::Unknown)
                toMake = move;

            searchInfo.IncrementDepth();
        }

        StopThinking();

        return toMake;
    }

    int Search::searchRoot(int depth, int alpha, int beta, Move& moveToMake, Board& board)
    {
        int score;
        int startTime = searchInfo.ElapsedTime();

        MoveSelector moves(board, searchInfo);
        MoveGenerator::GetLegalMoves(moves.moves, moves.count, board);

        // chopper pruning
        if (moves.count == 1)
        {
            moveToMake = moves.Next();
            return alpha;
        }

        moves.Sort<false>();

        int i = 0;
        for (auto move = moves.First(); !move.IsNull(); move = moves.Next(), i++)
        {
            if ((searchInfo.TimeOver() || StopSignal))
                return Constants::Unknown;

            board.MakeMove(move);
            if (i == 0) // leftmost node
                score = -Search::search<true>(depth-1, -beta, -alpha, 1, board); // pv node
            else
                score = -Search::search<false>(depth-1, -beta, -alpha, 1, board); // cut node
            board.UndoMove(move);

            if (score > alpha)
            {
                moveToMake = move;
                if (score >= beta)
                {
                    if (sendOutput)
                        Uci::SendCommand<Command::Info>(GetInfo(board, moveToMake, beta, depth, startTime)); // sends info to the gui

                    return beta;
                }

                alpha = score;
            }
        }

        if (sendOutput)
            Uci::SendCommand<Command::Info>(GetInfo(board, moveToMake, alpha, depth, startTime)); // sends info to the gui

        return alpha;
    }

    template<bool pv>
    int Search::search(int depth, int alpha, int beta, int ply, Board& board)
    {
        searchInfo.VisitNode();

        ScoreType bound = ScoreType::Alpha;
        bool futility = false;
        bool extension = false;
        int score;
        int legal = 0;
        Move best = Constants::NullMove;

        if (searchInfo.Nodes() % 10000 == 0) // every 10000 nodes visited we check for time expired
            if(searchInfo.TimeOver())
                StopSignal = true;

        if (StopSignal)
            return alpha;

        // Mate distance pruning
        alpha = std::max(alpha, -Constants::Mate + ply);
        beta = std::min(beta, Constants::Mate - ply - 1);
        if (alpha >= beta)
            return alpha;

        // Transposition table lookup
        auto hashHit = board.Table.Probe(board.zobrist, depth, alpha, beta);

        if( (score = hashHit.first) != TranspositionTable::Unknown)
            return score;
        best = hashHit.second;

        BitBoard attackers = board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove());
        if(attackers)
        {
            extension = true;
            ++depth;
        }

        // call to quiescence search
        if(depth == 0)
            return quiescence(alpha, beta, board);

        BitBoard pinned = board.PinnedPieces();

        // adaptive null move pruning
        // we skip this inside PV nodes
        if(board.AllowNullMove()
                //&& !pv
                && depth >= 3
                && !attackers
                && board.Material(board.SideToMove()) > Constants::Eval::EndGameMat)
        {
            int R = depth > 6 ? 3 : 2; // dynamic depth-based reduction

            // cut node
            board.MakeNullMove();
            score = -search<false>(depth - R - 1 , -beta, -beta+1, ply, board); // make a null-window search (we don't care by how much it fails high, if it does)
            board.UndoNullMove();

            if(score >= beta)
                return beta;
        }

        //        // internal iterative deepening (IID)
        //        if (depth >= 5 && best.IsNull() && pv)
        //        {
        //            int R = 2;

        //            if (board.AllowNullMove())
        //                board.ToggleNullMove();

        //            search<pv>(depth - R - 1, alpha, beta, ply, board); // make a full width search to find a new bestmove

        //            if (!board.AllowNullMove())
        //                board.ToggleNullMove();

        //            //Transposition table lookup
        //            auto hashHit = board.Table.Probe(board.zobrist, depth, alpha, beta);

        //            best = hashHit.second;
        //        }

        if (board.IsRepetition())
            return 0;

        int eval = Evaluation::Evaluate(board);

        //limited razoring
        if (!attackers
                //                && !pv
                && depth == 3
                && !extension
                && eval + 900 <= alpha
                )
        {
            depth=2;
        }

        //extended futility pruning condition
        if (!attackers
                //                && !pv
                && depth == 2
                && !extension
                && std::abs(alpha) < Constants::Mate-Constants::MaxPly
                && std::abs(beta) < Constants::Mate-Constants::MaxPly
                && eval + 500 <= alpha       // NEED to test other values
                )
        {
            futility = true;
        }

        //normal futility pruning condition
        if (!attackers
                //                && !pv
                && depth == 1
                && std::abs(alpha) < Constants::Mate-Constants::MaxPly
                && std::abs(beta) < Constants::Mate-Constants::MaxPly
                && eval + 250 <= alpha       // NEED to test other values
                )
        {
            futility = true;
        }


        MoveSelector moves(board, searchInfo);

        if (moves.count == 1) // forced move extension
        {
            extension = true;
            ++depth;
        }

        MoveGenerator::GetPseudoLegalMoves<false>(moves.moves, moves.count, attackers, board); // get captures and non-captures

        moves.Sort<false>(ply);
        moves.hashMove = best;

        // principal variation search
        bool capture;
        bool pruned = false;

        int moveNumber = 0;
        for(auto move = moves.First(); !move.IsNull(); move = moves.Next())
        {
            if(board.IsMoveLegal(move, pinned))
            {
                legal++;
                capture = board.IsCapture(move);

                board.MakeMove(move);

                // futility pruning application
                if (futility
                        && moveNumber > 0
                        && !capture
                        && !move.IsPromotion()
                        && !board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove())
                        )
                {
                    pruned = true;
                    board.UndoMove(move);
                    continue;
                }

                if (moveNumber == 0)
                {
                    score = -search<pv>(depth-1, -beta, -alpha, ply+1, board);
                }
                else
                {
                    register int R = 0;
                    register int N = depth >= 5 ? 4 : 2; // TO TEST

                    // late move reduction
                    if (moveNumber >= N
                            && depth >= 3
                            && !extension
                            && !capture
                            && !move.IsPromotion()
                            && !attackers
                            && move != searchInfo.FirstKiller(ply)
                            && move != searchInfo.SecondKiller(ply)
                            && !board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove())
                            )
                    {
                        R = 1;

                        if (eval + board.MaterialBalance(Utils::Piece::GetOpposite(board.SideToMove())) + depth*250 <= alpha)
                        {
                            R = 2;
                        }
                    }

                    score = -search<false>(depth-1-R, -alpha-1, -alpha, ply+1, board);

                    //                    if (score > alpha)
                    //                        score = -search<false>(depth-1, -alpha-1, -alpha, ply+1, board);

                    if (score > alpha)
                        score = -search<true>(depth-1, -beta, -alpha, ply+1, board);
                }

                board.UndoMove(move);

                if( score >= beta )
                {
                    if (move == best) // we don't want to save our hash move also as a killer move
                        return beta;

                    //killer moves and history heuristic
                    if(!board.IsCapture(move))
                    {
                        searchInfo.SetKillers(move, ply);
                        searchInfo.SetHistory(move, board.SideToMove(), depth);
                    }

                    // for safety, we don't save forward pruned nodes inside transposition table
                    if (!pruned)
                        board.Table.Save(board.zobrist, depth, beta, best, ScoreType::Beta);

                    return beta;   //  fail hard beta-cutoff
                }
                if(score > alpha)
                {
                    bound = ScoreType::Exact;
                    alpha = score; // alpha acts like max in MiniMax
                    best = move;
                }

                moveNumber++;
            }
        }

        // check for stalemate and checkmate
        if (legal == 0)
        {
            if (board.IsCheck())
                alpha = -Constants::Mate + ply; // return best score for the deepest mate
            else
                alpha = 0; // return draw score (TODO contempt factor)
        }

        // check for fifty moves rule
        if (board.HalfMoveClock() >= 100)
            alpha = 0;

        // for safety, we don't save forward pruned nodes inside transposition table
        if (!pruned)
            board.Table.Save(board.zobrist, depth, alpha, best, bound);

        return alpha;
    }

    // quiescence is called at horizon nodes (depth = 0)
    int Search::quiescence(int alpha, int beta, Board& board)
    {
        searchInfo.VisitNode();

        const BitBoard attackers = board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove());
        const bool inCheck = attackers;
        int stand_pat = 0; // to suppress warning
        int score;

        if(!inCheck)
        {
            stand_pat = Evaluation::Evaluate(board);
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
        }

        // TO TEST
        if (board.IsRepetition())
            return 0;

        const BitBoard pinned = board.PinnedPieces();

        MoveSelector moves(board, searchInfo);

        if(!inCheck)
            MoveGenerator::GetPseudoLegalMoves<true>(moves.moves, moves.count, attackers, board); // get all capture moves
        else
            MoveGenerator::GetPseudoLegalMoves<false>(moves.moves, moves.count, attackers, board); // get all evading moves

        moves.Sort<true>();

        for(auto move = moves.First(); !move.IsNull(); move = moves.Next())
        {
            // delta futility pruning
            if(!inCheck)
            {
                if (!move.IsPromotion() && (Constants::Piece::PieceValue[board.PieceOnSquare(move.ToSquare()).Type] + stand_pat + 200 <= alpha || board.See(move) < 0))
                    continue;
            }

            if (board.IsMoveLegal(move, pinned))
            {
                board.MakeMove(move);
                score = -quiescence( -beta, -alpha, board);
                board.UndoMove(move);

                if( score >= beta )
                    return beta;
                if( score > alpha )
                    alpha = score;
            }
        }

        return alpha;
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
        double delta = searchInfo.ElapsedTime() - lastTime;
        double nps = (delta > 0 ? searchInfo.Nodes() / delta : searchInfo.Nodes() / 1)*1000;

        info << "depth " << depth;

        if (std::abs(score) >= Constants::Mate - Constants::MaxPly)
        {
            int plies = Constants::Mate - std::abs(score) + 1;
            if (score < 0) // mated
                plies *= -1;

            info << " score mate " << plies / 2;
        }
        else
            info << " score cp " << score;

        info << " time " << searchInfo.ElapsedTime() << " nodes "
             << searchInfo.Nodes() << " nps " << static_cast<int>(nps) << " pv " << GetPv(board, toMake, depth);

        return info.str();
    }
}

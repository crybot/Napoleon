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

namespace Napoleon
{
	const int Search::AspirationValue = 25;
	bool Search::StopSignal = true;
	int Search::GameTime[2];
	int Search::MoveTime;

	SearchInfo Search::searchInfo;

	// direct interface to the client.
	// it sends the move to the uci gui
	void Search::StartThinking(SearchType type, Board& board)
	{
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
				assert(type == SearchType::TimePerMove);
				time = MoveTime;
			}

			searchInfo.NewSearch(time);
		}

		Move move = iterativeSearch(board);
		Uci::SendCommand<Command::Move>(move.ToAlgebraic());
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

			//            aspiration search
			temp = searchRoot(searchInfo.MaxDepth(), score - AspirationValue, score + AspirationValue, move, board);

			if (temp <= score - AspirationValue)
			{
				temp = searchRoot(searchInfo.MaxDepth(), -Constants::Infinity, score + AspirationValue, move, board);
			}
			else if (temp >= score + AspirationValue)
			{
				temp = searchRoot(searchInfo.MaxDepth(), score - AspirationValue, Constants::Infinity, move, board);
			}

			//            temp = searchRoot(i, -Constants::Infinity, Constants::Infinity, move, board);

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
		int moveIndex = 0;

		MoveSelector moves(board, searchInfo);
		MoveGenerator::GetLegalMoves(moves.moves, moves.count, board);

		// chopper pruning
		if (moves.count == 1)
		{
			moveToMake = moves.Next();
			return alpha;
		}

		moves.Sort();


		for (int i=0; i<moves.count; i++)
		{
			if ((searchInfo.TimeOver() || StopSignal))
				return Constants::Unknown;

			auto move = moves.Next();

			board.MakeMove(move);
			score = -Search::search(depth-1, -beta, -alpha, 1, board);
			board.UndoMove(move);

			if (score > alpha)
			{
				moveIndex = i;
				if (score >= beta)
				{
					moveToMake = move;
					Uci::SendCommand<Command::Info>(GetInfo(board, moveToMake, beta, depth, startTime)); // sends info to the gui
					return beta;
				}

				alpha = score;
			}
		}

		moveToMake = moves[moveIndex];
		Uci::SendCommand<Command::Info>(GetInfo(board, moveToMake, alpha, depth, startTime)); // sends info to the gui

		return alpha;
	}

	int Search::search(int depth, int alpha, int beta, int ply, Board& board)
	{
		searchInfo.VisitNode();

		ScoreType bound = ScoreType::Alpha;
		bool futility = false;
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
			++depth;

		// call to quiescence search
		if(depth == 0)
			return quiescence(alpha, beta, board);

		BitBoard pinned = board.PinnedPieces();

		// enhanced deep razoring
		if (depth < 4
			&& !attackers
			&& board.Material(board.SideToMove()) > Constants::Eval::EndGameMat
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
			&& board.Material(board.SideToMove()) > Constants::Eval::EndGameMat)
		{
			int R = depth > 5 ? 3 : 2; // dynamic depth-based reduction

			board.MakeNullMove();
			score = -search(depth - R - 1 , -beta, -beta+1, ply+1, board); // make a null-window search (we don't care by how much it fails high, if it does)
			board.UndoNullMove();

			if(score >= beta)
				return beta;
		}

		// internal iterative deepening (IID)
		if (depth > 4 && best.IsNull())
		{
			int R = 2;

			search(depth - R - 1, -Constants::Infinity, Constants::Infinity, ply, board); // make a full width search to find a new bestmove

			//Transposition table lookup
			auto hashHit = board.Table.Probe(board.zobrist, depth, alpha, beta);

			best = hashHit.second;
		}

		// make best move (hash move)
		if(!best.IsNull())
		{
			assert(board.IsMoveLegal(best, pinned));
			legal++;
			board.MakeMove(best);
			score = -search(depth - 1, -beta, -alpha, ply + 1, board);
			board.UndoMove(best);

			if(score >= beta)
				return beta;

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
			&& std::abs(alpha) < Constants::Mate-Constants::MaxPly
			&& Evaluation::Evaluate(board) + futilityMargin(depth) <= alpha)
		{
			futility = true;
		}

		MoveSelector moves(board, searchInfo);

		MoveGenerator::GetPseudoLegalMoves<false>(moves.moves, moves.count, attackers, board); // get captures and non-captures

		moves.Sort(ply);

		// principal variation search
		bool PVS = true;
		bool capture;

		for(int i=0; i<moves.count; i++)
		{
			auto move = moves.Next();

			if(board.IsMoveLegal(move, pinned))
			{
				legal++;
				capture = board.IsCapture(move);

				//TO TEST
				//if(capture)
				//{
				//	if (!attackers && !move.IsPromotion() && depth < 4 && board.See(move) < 0)
				//		continue;
				//}

				board.MakeMove(move);

				// extended futility pruning application
				if (futility
					&& i > 0
					&& !capture
					&& !move.IsPromotion()
					&& !board.KingAttackers(board.KingSquare(board.SideToMove()), board.SideToMove())
					)
				{
					board.UndoMove(move);
					continue;
				}

				if (PVS)
				{
					score = -search(depth-1, -beta, -alpha, ply+1, board);
				}
				else
				{
					score = -search(depth-1, -alpha-1, -alpha, ply+1, board);
					if (score > alpha/* && score < beta*/)
						score = -search(depth-1, -beta, -alpha, ply+1, board);
				}

				board.UndoMove(move);

				if( score >= beta )
				{
					//killer moves and history heuristic
					if(!board.IsCapture(move))
					{
						searchInfo.SetKillers(move, ply);
						searchInfo.SetHistory(move, board.SideToMove(), depth);
					}

					board.Table.Save(board.zobrist, depth, beta, best, ScoreType::Beta);

					return beta;   //  fail hard beta-cutoff
				}
				if(score > alpha)
				{
					PVS = false;
					bound = ScoreType::Exact;
					alpha = score; // alpha acts like max in MiniMax
					best = move;
				}
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

			//        big delta futility pruning
			if ( stand_pat < alpha - Delta)
				return alpha;

			if( alpha < stand_pat )
				alpha = stand_pat;
		}

		//if (board.IsRepetition())
		//	return 0;

		const BitBoard pinned = board.PinnedPieces();

		MoveSelector moves(board, searchInfo);

		if(!inCheck)
			MoveGenerator::GetPseudoLegalMoves<true>(moves.moves, moves.count, attackers, board); // get all capture moves
		else
			MoveGenerator::GetPseudoLegalMoves<false>(moves.moves, moves.count, attackers, board); // get all evading moves

		moves.Sort();

		for(int i=0; i<moves.count; i++)
		{
			auto move = moves.Next();

			// delta futility pruning
			if(!inCheck)
			{
				if ((Constants::Piece::PieceValue[board.PieceOnSquare(move.ToSquare()).Type] + stand_pat + 200 < alpha || board.See(move) < 0) && !move.IsPromotion())
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

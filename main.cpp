#include <iostream>
#include "utils.h"
#include "constants.h"
#include "stopwatch.h"
#include "piece.h"
#include "compassrose.h"
#include "board.h"
#include "pawn.h"
#include "knight.h"
#include "king.h"
#include "rook.h"
#include "bishop.h"
#include "queen.h"
#include "movedatabase.h"
#include "fenstring.h"
#include "movegenerator.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <ctime>
#include <vector>
#include "benchmark.h"
#include "search.h"
#include "evaluation.h"
#include "console.h"
#include "uci.h"

using namespace Napoleon;
using namespace std;

void Divide(int depth, Board& board, Benchmark bench)
{
    int pos = 0;
    unsigned long long total = 0;
    int temp = 0;
    int NumMoves = 0;

    Move moves[Constants::MaxMoves];
    MoveGenerator::GetLegalMoves(moves, pos, board);

    std::cout << "Move\tNodes" << std::endl;

    for (int i = 0; i < pos; i++)
    {
        board.MakeMove(moves[i]);
        temp = bench.Perft(depth - 1, board);
        total += temp;
        std::cout << moves[i].ToAlgebraic() << "\t" << temp << std::endl;
        board.UndoMove(moves[i]);
        NumMoves++;
    }

    std::cout << "Total Nodes: " << total << std::endl;
    std::cout << "Moves: " << NumMoves << std::endl;
}

void SearchMove(int depth, Board& board)
{
    Move move;
    Search::searchRoot(depth, -Constants::Infinity, Constants::Infinity, move, board);

    board.MakeMove(move);
    board.Display();
    cout << "I.A. Move: " << move.ToAlgebraic() << endl;
}

int main()
{
    //#define MAIN

#ifndef MAIN

    Uci::Start();

#endif

#ifdef MAIN
    using namespace Constants;
    using namespace Constants::Squares;

    MoveDatabase::InitAttacks();
    Zobrist::Init();

    StopWatch watch;
    Board board;
    Benchmark bench;
    board.LoadGame();

    //    board.Display();

    //    Utils::BitBoard::Display(board.bitBoardSet[board.SideToMove][PieceType::Pawn]);
    //    Utils::BitBoard::Display(board.bitBoardSet[board.SideToMove ^ 1][PieceType::Pawn]);

    //    cout << "Size HashEntry: " << sizeof(HashEntry) << endl;

    //    board.Display();
    //    cin.get();

    //    Move move(Constants::Squares::IntE4, Constants::Squares::IntD5, Constants::CaptureMask);

    //    assert(!move.IsEnPassant());

    //    Utils::BitBoard::Display(board.bitBoardSet[Color::White][PieceType::Pawn]);
    //    cin.get();
    //    Utils::BitBoard::Display(board.bitBoardSet[Color::Black][PieceType::Pawn]);
    //    cin.get();

    //    board.MakeMove(move);
    //    board.Display();
    //    cin.get();

    //    Utils::BitBoard::Display(board.bitBoardSet[Color::White][PieceType::Pawn]);
    //    cin.get();
    //    Utils::BitBoard::Display(board.bitBoardSet[Color::Black][PieceType::Pawn]);
    //    cin.get();

    //    board.UndoMove(move);
    //    board.Display();
    //    cin.get();

    //    Utils::BitBoard::Display(board.bitBoardSet[Color::White][PieceType::Pawn]);
    //    cin.get();
    //    Utils::BitBoard::Display(board.bitBoardSet[Color::Black][PieceType::Pawn]);
    //    cin.get();


    //    cout << move.ToAlgebraic() << endl;

    //    if (move.IsCapture())
    //        cout << "Cattura" << endl;

    //    cin.get();

    while(1)
    {
        cout << "Napoleon: ";
        vector<string> fields;
        string cmd;
        std::getline(cin, cmd);

        boost::split(fields, cmd, boost::is_any_of(" "));

        if (fields[0] == "perft")
        {
            if (fields.size() > 1)
            {
                int depth = boost::lexical_cast<int>(fields[1]);
                watch.Start();
                cout << "Perft(" << depth << "): ";
                cout << "Total Nodes: " << bench.Perft(depth, board) << endl;
                cout << "Time (ms): " << watch.Stop().ElapsedMilliseconds() << endl;
            }
        }

        else if (fields[0] == "divide")
        {
            if (fields.size() > 1)
            {
                watch.Start();
                Divide(boost::lexical_cast<int>(fields[1]), board, bench);
                cout << "Time (ms): " << watch.Stop().ElapsedMilliseconds() << endl;
            }
        }

        else if (fields[0] == "search")
        {
            if (fields.size() > 1)
            {
                int depth = boost::lexical_cast<int>(fields[1]);
                watch.Start();
                SearchMove(depth, board);
                cout << "Time (ms): " << watch.Stop().ElapsedMilliseconds() << endl;
                //                cout << "PV: " << board.Table.Table[board.zobrist % board.Table.Size].BestMove.ToAlgebraic() << endl;
            }
        }

        else if (fields[0] == "setboard")
        {
            if (fields.size() >= 5)
            {
                board.LoadGame(cmd.substr(fields[0].size()+1));
            }
        }

        else if (cmd == "display")
        {
            board.Display();
        }

        else if (cmd == "new")
        {
            board = Board();
            board.LoadGame();
        }

        else if (fields[0] == "move")
        {
            if (fields.size() == 2)
            {
                Move move = board.ParseMove(fields[1]);
                if (move != Constants::NullMove)
                {

                    board.MakeMove(move);
                    board.Display();
                    //                    Move move = Search::iterativeSearch(board);
                    //                    board.MakeMove(move);
                    //                    board.Display();
                }
                else
                {
                    cout << "Invalid Move" << endl;
                }
            }
        }

        //        else if (fields[0] == "undo")
        //        {
        //            board.UndoMove(board.moves[board.CurrentPly - 1]);
        //            board.Display();
        //        }

        else if (fields[0] == "bench")
        {
            bench.Start();
        }
        else if (fields[0] == "bench2")
        {
            bench.CutoffTest();
        }

        else if (fields[0] == "play")
        {
            int pos;
            watch.Start();
            do
            {
                Move legalMoves[Constants::MaxMoves + 2];
                pos = 0;
                MoveGenerator::GetLegalMoves(legalMoves, pos, board);
                if (pos > 0)
                {
                    Search::iterativeSearch(board);
                }

            }while( pos > 0);

            board.Display();
            cout << Console::Red << "#Mate for " << (board.SideToMove == Color::White ? "White#" : "Black#");
            cout << endl << "Time (ms): " << watch.Stop().ElapsedMilliseconds() << endl;

        }

        else if (fields[0] == "iterate")
        {
            watch.Start();
            Search::StartThinking(board);
            cout << "Time (ms): " << watch.Stop().ElapsedMilliseconds() << endl;
            //            cout << "PV: " << board.Table.Table[board.zobrist % board.Table.Size].BestMove.ToAlgebraic() << endl;
        }

        else if (fields[0] == "zobrist")
        {
            cout << "Zobrist: " << board.zobrist << endl;
        }

        else if (fields[0] == "fen")
        {
            cout << "FEN: " << board.GetFen() << endl;
        }

        else
        {
            cout << "incorrect command: " << cmd << endl;
        }

        cout << Console::Reset << endl;
    }
#endif

    return 0;
}

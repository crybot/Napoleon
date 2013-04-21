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
#include "benchmark.h"
#include "search.h"
#include "evaluation.h"
#include "console.h"

using namespace Napoleon;
using namespace std;

void Divide(int depth, Board& board, Benchmark bench)
{
    int pos = 0;
    unsigned long long total = 0;
    int temp = 0;
    int NumMoves = 0;

    Move moves[Constants::MaxMoves + 2];
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
    int max = -32767;
    int pos = 0;
    int move = 0;
    int score;
    Move moves[Constants::MaxMoves + 2];

    MoveGenerator::GetLegalMoves(moves, pos, board);

    if (pos == 0)
    {
        cout << Console::Red << "#CheckMate for Black#";
        cout << Console::Reset << endl;
    }
    else
    {

        for (int i=0; i<pos; i++)
        {

            board.MakeMove(moves[i]);
            score = -Search::search(depth-1, -32767, 32767, board);
            board.UndoMove(moves[i]);
            if( score > max )
            {
                max = score;
                move = i;
            }
        }

        board.MakeMove(moves[move]);
        board.Display();
        cout << "I.A. Move: " << moves[move].ToAlgebraic() << endl;

        pos = 0;
        MoveGenerator::GetLegalMoves(moves, pos, board);
        if (pos == 0)
        {
            cout << Console::Red << "#CheckMate for White#";
            cout << Console::Reset << endl;
        }


    }
}

int main()
{

    using namespace Constants;
    using namespace Constants::Squares;

    StopWatch watch;
    Board board;
    Benchmark bench;
    board.Equip();

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
                cout << "Nodes per second: " << (board.nps ) << endl;
                board.nps = 0;
            }
        }

        else if (fields[0] == "setboard")
        {
            if (fields.size() >= 5)
            {
                board.LoadGame(FenString(cmd.substr(fields[0].size()+1)));
            }
        }

        else if (cmd == "display")
        {
            board.Display();
        }

        else if (cmd == "new")
        {
            board = Board();
            board.Equip();
        }

        else if (fields[0] == "move")
        {
            if (fields.size() == 2)
            {
                MoveList legalMoves;
                MoveGenerator::GetLegalMoves(legalMoves.List(), legalMoves.size, board);
                Move move = board.ParseMove(fields[1], legalMoves);
                if (legalMoves.size > 0)
                {
                    if (move != Constants::NullMove)
                    {

                        board.MakeMove(move);
                        board.Display();
                        if (legalMoves.size < 25)
                            SearchMove(6, board);
                        else
                            SearchMove(5, board);
                    }
                    else
                    {
                        cout << "Invalid Move" << endl;
                    }
                }
                else
                {
                    cout << Console::Red << "#Mate for White#";
                }
            }
        }

        else if (fields[0] == "bench")
        {
            bench.Start();
        }

        else if (fields[0] == "play")
        {
            int pos;
            do
            {
                Move legalMoves[Constants::MaxMoves + 2];
                pos = 0;
                MoveGenerator::GetLegalMoves(legalMoves, pos, board);
                if (pos > 0)
                {
                    if (pos < 25)
                        SearchMove(5, board);
                    else
                        SearchMove(6, board);
                }

            }while( pos > 0);

            cout << Console::Red << "#Mate for " << (board.SideToMove == PieceColor::White ? "White #" : "Black #");

        }
        else
        {
            cout << "incorrect command: " << cmd << endl;
        }

        cout << endl;
    }

    return 0;
}

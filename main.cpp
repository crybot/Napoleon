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
                board.MakeMove(board.ParseMove(fields[1]));
            }
        }

        else if (fields[0] == "undo")
        {
            if (fields.size() == 2)
            {
                board.UndoMove(board.ParseMove(fields[1]));
            }
        }

        else if (fields[0] == "bench")
        {
            bench.Start();
        }
        else
        {
            cout << "incorrect command: " << cmd << endl;
        }
        cout << endl;
    }

    return 0;
}

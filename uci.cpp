
#include "uci.h"
#include <iostream>
#include <string>
#include "search.h"
#include "fenstring.h"
#include "board.h"
#include "stopwatch.h"
#include "benchmark.h"

namespace Napoleon
{
    using namespace std;

    Board Uci::board;
    thread Uci::search;

    void Uci::Start()
    {
        SendCommand<Command::Generic>("--------Napoleon Engine--------");
        cout.setf(ios::unitbuf);// Make sure that the outputs are sent straight away to the GUI

        string line;
        string cmd;
        Search::Table.SetSize(512);
        Search::InitializeThreads();
        bool exit = false;

        while(!exit && getline(cin, line))
        {
            istringstream stream(line);
            stream >> cmd;

            if (cmd == "uci")
            {
                SendCommand<Command::Generic>("id name Napoleon");
                SendCommand<Command::Generic>("id author Marco Pampaloni");
                SendCommand<Command::Generic>("option name Hash type spin default 1 min 1 max 1024");
                SendCommand<Command::Generic>("option name Threads type spin default 2 min 1 max 8");
                SendCommand<Command::Generic>("uciok");
            }
            else if (cmd == "setoption")
            {
                string token;
                stream >> token; // "name"
                stream >> token;

                if(token == "Hash")
                {
                    stream >> token; // "value"
                    stream >> token;
                    Search::Table.SetSize(std::stoi(token));
                }
                else if (token == "Threads") 
                {
                    int parallel_threads;
                    stream >> token; // "value"
                    stream >> parallel_threads;
                    Search::InitializeThreads(parallel_threads);
                }
            }
            else if (cmd == "quit")
            {
                SendCommand<Command::Generic>("Bye Bye");
                Search::KillThreads();
                exit = true;
            }
            else if (cmd == "isready")
            {
                SendCommand<Command::Generic>("readyok");
            }
            else if (cmd == "ucinewgame")
            {
                Search::Table.Clear();
            }
            else if (cmd == "stop")
            {
                Search::StopThinking();
            }
            else if (cmd == "perft")
            {
                Benchmark bench(board);

                int depth;
                stream >> depth;

                StopWatch watch = StopWatch::StartNew();

                cout << "Perft(" << depth << "): ";
                cout << "Total Nodes: " << bench.Perft(depth) << endl;
                cout << "Time (ms): " << watch.ElapsedMilliseconds() << endl;
            }
            else if (cmd == "position")
            {
                Move move;
                string token;
                stream >> token;

                if (token == "startpos")
                {
                    board.LoadGame();
                    stream >> token;
                }
                else if (token == "fen")
                {
                    string fen;
                    while (stream >> token && token != "moves")
                        fen += token + " ";

                    board.LoadGame(fen);
                }

                while (stream >> token && !(move = board.ParseMove(token)).IsNull())
                {
                    board.MakeMove(move);
                }
            }
            else if (cmd == "ECM")
            {
                int depth;
                stream >> depth;

                Benchmark bench(board);
                bench.Start(depth);
            }
            else if (cmd == "disp")
            {
                board.Display();
            }
            else if (cmd == "eval")
            {
                std::cout << Evaluation::Evaluate(board) << std::endl;
            }
            else if (cmd == "go")
            {
                if (Search::StopSignal)
                    go(stream);
            }
        }
    }

    void Uci::go(istringstream& stream)
    {
        string token;
        SearchType type;
        bool san = false;

        while(stream >> token)
        {
            if (token == "depth")
            {
                stream >> Search::depth_limit;
                type = SearchType::Infinite;
            }
            else if (token == "movetime")
            {
                stream >> Search::MoveTime;
                type = SearchType::TimePerMove;
            }

            else if (token == "wtime")
            {
                stream >> Search::GameTime[PieceColor::White];
                type = SearchType::TimePerGame;
            }
            else if (token == "btime")
            {
                stream >> Search::GameTime[PieceColor::Black];
                type = SearchType::TimePerGame;
            }
            else if (token == "infinite")
            {
                type = SearchType::Infinite;
            }

        }

        search = thread(Search::StartThinking, type, ref(board), true, san);
        search.detach();
    }

}

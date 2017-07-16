#include <iostream>
#include <string>
#include "uci.h"
#include "search.h"
#include "fenstring.h"
#include "board.h"
#include "stopwatch.h"
#include "benchmark.h"
#include "evaluation.h"
#include "tuner.h"
#include <fstream>

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
                SendCommand<Command::Generic>("option name Hash type spin default 1 min 1 max 131072"); // max 128 GB
                SendCommand<Command::Generic>("option name Threads type spin default 1 min 1 max 8");

                for (auto i=0; i<Search::Parameters::MAX; i++)
                {
                    SendCommand<Command::Generic>("option name " + Search::param_name[i] + " type spin default 1 min 1 max 1024");
                }
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

                else
                {
                    for (auto i=0; i<Search::Parameters::MAX; i++)
                    {
                        if (token == Search::param_name[i])
                        {
                            stream >> token; // "value"
                            stream >> Search::param[i];
                            break;
                        }
                    }
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
                //Search::StopThinking();
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
                Evaluation::PrintEval(board);
                std::cout << Evaluation::Evaluate(board) << std::endl;
            }
            else if (cmd == "go")
            {
                //if (Search::StopSignal)
                go(stream);
            }
            else if (cmd == "ponderhit")
            {
                Search::PonderHit = true;
            }
            else if (cmd == "tune")
            {
                Tuner tuner;
                tuner.Tune();
            }
        }
    }

    void Uci::go(istringstream& stream)
    {
        string token;
        SearchType type = SearchType::TimePerGame;
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
                //type = SearchType::TimePerGame;
            }
            else if (token == "btime")
            {
                stream >> Search::GameTime[PieceColor::Black];
                //type = SearchType::TimePerGame;
            }
            else if (token == "infinite")
            {
                type = SearchType::Infinite;
            }
            else if (token == "ponder")
            {
                type = SearchType::Ponder;
            }

        }

        search = thread(Search::StartThinking, type, ref(board), true, san);
        search.detach();
    }

}

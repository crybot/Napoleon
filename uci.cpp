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
        while(getline(cin, line))
        {
            istringstream stream(line);
            stream >> cmd;

            if (cmd == "uci")
            {
                SendCommand<Command::Generic>("id name Napoleon");
                SendCommand<Command::Generic>("id author Crybot");
                SendCommand<Command::Generic>("uciok");
            }
            else if (cmd == "quit")
            {
                SendCommand<Command::Generic>("Bye Bye");
                break;
            }
            else if (cmd == "isready")
            {
                SendCommand<Command::Generic>("readyok");
            }
            else if (cmd == "ucinewgame")
            {
                board.Table.Clear();
            }
            else if (cmd == "stop")
            {
                Search::StopThinking();
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
            else if (cmd == "go")
            {
                if (Search::Task == Stop)
                    go(stream);
            }
        }
    }

    void Uci::go(istringstream& stream)
    {
        string token;
        Search::MoveTime = false;

        while(stream >> token)
        {
            if (token == "wtime") stream >> Search::Time[PieceColor::White];
            else if (token == "btime") stream >> Search::Time[PieceColor::Black];
            else if (token == "infinite") Search::Task = Infinite;
            else if (token == "movetime")
            {
                stream >> Search::ThinkTime;
                Search::MoveTime = true;
            }
        }

        search = thread(Search::StartThinking, ref(board));
        search.detach();
    }

}

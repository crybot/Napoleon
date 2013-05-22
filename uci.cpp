#include "uci.h"
#include <iostream>
#include <string>
#include "search.h"
#include "fenstring.h"
#include "board.h"

namespace Napoleon
{
    using namespace std;
    Board Uci::board;

    void Uci::Start()
    {
        string line;
        cout.setf(ios::unitbuf);// Make sure that the outputs are sent straight away to the GUI

        while(ReadCommand()) { };
    }

    bool Uci::ReadCommand()
    {
        string line;
        string cmd;
        bool repeat = getline(cin, line);

        if (!repeat)
            return false;

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
            return false;
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
        if (cmd == "position")
        {
            Move move;
            string token;
            stream >> token;

            if (token == "startpos")
            {
                board.Equip();
                stream >> token;
            }
            else if (token == "fen")
            {
                string fen;
                while (stream >> token && token != "moves")
                    fen += token + " ";

                board.LoadGame(FenString(fen));
            }

            while (stream >> token && !(move = board.ParseMove(token)).IsNull())
            {
                board.MakeMove(move);
            }
        }
        else if (cmd == "go")
        {
            Search::StartThinking(board);
        }

        return repeat;
    }
}

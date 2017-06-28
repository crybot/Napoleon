#ifndef UCI_H
#define UCI_H
#include <sstream>
#include <iostream>
#include <thread>

namespace Napoleon
{
    enum class Command
    {
        Generic, Move, Info
    };

    class Board;
    namespace Uci
    {
        void Start();

        template<Command>
        void SendCommand(std::string, std::string="");   
        void go(std::istringstream&);

        extern Board board;
        extern std::thread search;
    }

    template<Command cmdType>
    void Uci::SendCommand(std::string command, std::string ponder)
    {
        switch(cmdType)
        {
        case Command::Generic:
            std::cout << command << std::endl;
            break;
        case Command::Move:
            std::cout << "bestmove " << command;
            if(!ponder.empty()) std::cout << " ponder " << ponder;
            std::cout << std::endl;
            break;
        case Command::Info:
            std::cout << "info " << command << std::endl;
            break;
        default:
            std::cout << std::endl;
            break;
        }
    }
}

#endif // UCI_H

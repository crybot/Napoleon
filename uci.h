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
        void SendCommand(std::string);   
        void go(std::istringstream&);

        extern Board board;
        extern std::thread search;
    }

    template<Command cmdType>
    void Uci::SendCommand(std::string command)
    {
        switch(cmdType)
        {
        case Command::Generic:
            std::cout << command << std::endl;
            break;
        case Command::Move:
            std::cout << "bestmove " << command << std::endl;
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

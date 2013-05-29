#ifndef UCI_H
#define UCI_H
#include <sstream>
#include <iostream>

namespace Napoleon
{
    namespace Command
    {
        typedef int Type;
        const Type Generic = 0;
        const Type Move = 1;
        const Type Info = 2;
    }

    class Board;
    namespace Uci
    {
        void Start();
        bool ReadCommand();
        template<Command::Type>
        void SendCommand(std::string);   
        void Go(std::istringstream&);

        extern Board board;
    }

    template<Command::Type cmdType>
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

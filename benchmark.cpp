#include "benchmark.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "fenstring.h"
#include "movegenerator.h"
#include "constants.h"
#include "console.h"
#include "search.h"

namespace Napoleon
{
    Benchmark::Benchmark()
    {
        board = Board();
        board.Equip();
    }

    void Benchmark::Start()
    {
        std::ifstream ff("perft.epd");
        std::vector<std::string> strings;
        std::vector<std::string> fields;
        std::string buff;
        int err = 0;
        unsigned long long result;
        unsigned long long excpected;
        int depth;

        while (getline(ff, buff))
        {
            strings.push_back(buff);
        }

        for (unsigned i=0; i<strings.size(); i++)
        {
            boost::split(fields, strings[i], boost::is_any_of(";"));
            board.LoadGame(FenString(fields[0]));

            std::cout << Console::Yellow << "Position: " << i+1 << std::endl << fields[0] << std::endl;
            for (unsigned l=1; l<fields.size(); l++)
            {
                std::istringstream buffer(fields[l].substr(1));
                buffer >> depth;
                buffer >> excpected;

                std::cout << Console::Reset <<  "Perft " << depth << ": " << std::endl;

                result = Perft(depth, board);

                if(result != excpected)
                {
                    std::cout << Console::Red << "Nodes: " << result << std::endl;
                    std::cout << Console::Red << "Expected: " << excpected << std::endl;
                    err++;
                }
                else
                {
                    std::cout << Console::Green << "Nodes: " << result << std::endl;
                    std::cout << Console::Green << "Expected: " << excpected << std::endl;
                }
            }
        }

        if (err == 0)
            std::cout << Console::Green << "Test Succesfully Passed!" << std::endl;
        else
            std::cout << Console::Red << "Test Not Passed!" << std::endl;
        std::cout << Console::Reset;
    }

    void Benchmark::CutoffTest()
    {
        std::ifstream ff("perft.epd");
        std::vector<std::string> strings;
        std::vector<std::string> fields;
        std::string buff;
        int err = 0;
        unsigned long long result;
        unsigned long long excpected;
        int depth;

        while (getline(ff, buff))
        {
            strings.push_back(buff);
        }

        for (unsigned i=0; i<50; i++)
        {
            boost::split(fields, strings[i], boost::is_any_of(";"));
            board.LoadGame(FenString(fields[0]));

            Search::IterativeSearch(board);
            std::cout << i << std::endl;

        }

        std::cout << "Cutoff on first move: " << board.FirstMoveCutoff << std::endl;
        std::cout << "Total Cutoffs: " << board.TotalCutoffs << std::endl;
        std::cout <<  Console::Green << "First Move Cutoff percentage: " << ((float)board.FirstMoveCutoff / (float)board.TotalCutoffs) * 100 << "%" << std::endl;
        std::cout <<  Console::Reset << std::endl;



    }


    unsigned long long Benchmark::Perft(int depth, Board& board)
    {
        int pos = 0;
        int count;
        Move move;
        Move moves[Constants::MaxMoves + 2];

        unsigned long long nodes = 0;



        MoveGenerator::GetLegalMoves(moves, pos, board);

        if (depth == 1)
        {

            return pos;
        }

        if (depth == 0)
            return 1;

        for (int i = 0; i < pos; i++)
        {
            board.MakeMove(moves[i]);
            nodes += Perft(depth - 1, board);
            board.UndoMove(moves[i]);
        }


        return nodes;
    }

}

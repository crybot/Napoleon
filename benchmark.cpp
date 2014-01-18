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
#include "board.h"
#include "stopwatch.h"


// ONLY USEFUL FOR DEBUG
namespace Napoleon
{
    Benchmark::Benchmark(Board& board)
        :board(board)
    {

    }

    void Benchmark::Start(int depth)
    {
        std::ifstream fstream("ECM.epd");

        std::string str = "";
        std::vector<std::string> strings;
        std::vector<std::string> fields;
        std::string buff;
        int nodesVisited = 0;
        int correct = 0;

        while (getline(fstream, buff))
        {
            strings.push_back(buff);
        }

        StopWatch watch = StopWatch::StartNew();

        int i = 1;
        for (std::string line : strings)
        {
            std::cout << i++ << " ";
            boost::split(fields, line, boost::is_any_of(";"));

            FenString epd(fields[0]);
            board.LoadGame(epd.FullString);

            Search::searchInfo.SetDepthLimit(depth);
            Move move = Search::StartThinking(SearchType::Infinite, board, false);

            if(move.ToSan(board) == epd.BestMove)
                correct++;

            nodesVisited += Search::searchInfo.Nodes();
        }


        std::cout << str << std::endl;
        std::cout << "correct: " << correct << "/" << strings.size() << std::endl;
        std::cout << "rate: " << ((float)correct/(float)strings.size())*100 << std::endl;
        std::cout << "nodes visited: " << nodesVisited << std::endl;
        std::cout << "elapsed time(ms): " << watch.ElapsedMilliseconds() << std::endl;

        fstream.close();
    }

    void Benchmark::CutoffTest()
    {
        std::ifstream ff("perft.epd");
        std::vector<std::string> strings;
        std::vector<std::string> fields;
        std::string buff;

        while (getline(ff, buff))
        {
            strings.push_back(buff);
        }

        for (unsigned i=0; i<50; i++)
        {
            boost::split(fields, strings[i], boost::is_any_of(";"));
            board.LoadGame(fields[0]);

            Search::StartThinking(SearchType::Infinite, board);
            std::cout << i << std::endl;
        }

        std::cout << "Cutoff on first move: " << board.FirstMoveCutoff << std::endl;
        std::cout << "Total Cutoffs: " << board.TotalCutoffs << std::endl;
        std::cout <<  Console::Green << "First Move Cutoff percentage: " << ((float)board.FirstMoveCutoff / (float)board.TotalCutoffs) * 100 << "%" << std::endl;
        std::cout <<  Console::Reset << std::endl;

    }

    unsigned long long Benchmark::Perft(int depth)
    {
        int pos = 0;
        Move moves[Constants::MaxMoves];

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
            nodes += Perft(depth - 1);
            board.UndoMove(moves[i]);
        }

        return nodes;
    }

}

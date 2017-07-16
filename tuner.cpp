#include <random>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <algorithm>
#include "tuner.h"
#include "fenstring.h"
#include "encoder.h"
#include "search.h"
#include <boost/process.hpp>

using namespace Evolution;

namespace Napoleon
{
    //std::vector<std::vector<std::string>> fenstrings;
    //std::vector<std::string> strings;
    std::vector<Chromosome> population;
    const int pop_size = 10;

    void Tuner::load_file()
    {
        /*
        std::string buff;
        std::ifstream fstream("ECM.epd");

        while (getline(fstream, buff))
        {
            strings.push_back(buff);
        }
        fstream.close();

        for (std::string line : strings)
        {
            std::vector<std::string> temp;
            boost::split(temp, line, boost::is_any_of(";"));
            fenstrings.push_back(temp);
        }
        */
    }

    void Tuner::Tune()
    {

        Board board;
        Encoder encoder;

        encoder.AddField(Search::RAZOR1, 7);
        encoder.AddField(Search::RAZOR2, 2);
        encoder.AddField(Search::RAZOR3, 9);
        encoder.AddField(Search::REVERSENULL_DEPTH, 4);
        encoder.AddField(Search::REVERSENULL1, 9);
        encoder.AddField(Search::REVERSENULL2, 8);
        encoder.AddField(Search::NULLPRUNE1, 4);
        encoder.AddField(Search::NULLPRUNE2, 4);
        encoder.AddField(Search::NULLPRUNE3, 4);
        encoder.AddField(Search::FUTILITY1, 10);
        encoder.AddField(Search::FUTILITY2, 10);
        encoder.AddField(Search::IID_DEPTH, 4);
        encoder.AddField(Search::IID1, 4);
        encoder.AddField(Search::LMR_DEPTH1, 3);
        encoder.AddField(Search::LMR_DEPTH2, 3);
        encoder.AddField(Search::LMR_DEPTH3, 3);
        encoder.AddField(Search::LMR_R1, 3);
        encoder.AddField(Search::LMR_R2, 3);
        encoder.AddField(Search::LMR_MARGIN, 5);
        size_t bits = encoder.Bits();

        for (auto i=0; i<pop_size/2; ++i)
            population.push_back(encoder.Encode(Search::param));

        for (auto i=pop_size/2; i < pop_size; ++i)
            population.push_back(Evolution::Random(bits));

        for (auto p : population)
            std::cout << p << std::endl;

        int generation = 0;
        Chromosome best(bits);
        while(true)
        {
            std::cout << "generation: " << generation << std::endl;
            int ith = 1;
            for (auto& p : population)
            {
                //if (p.fitness == 0) // IT MAY REDUCE EXPLORATION
                p.fitness = fitness(p, board, encoder);

                std::cout << "#" << ith++ << "/" << pop_size << " elo difference: " << p.fitness << std::endl;
            }

            //selection 
            std::sort(population.begin(), population.end(), 
                    [](Chromosome a, Chromosome b){
                    return a.fitness > b.fitness;
                    });

            auto rand1 = population[random() % (pop_size - pop_size/2) + pop_size/2];
            //auto rand2 = population[random() % (pop_size - pop_size/2) + pop_size/2];
            best = population.front();

            population.resize(pop_size/2);
            population.push_back(rand1);
            //population.push_back(rand2);
            std::random_shuffle(population.begin(), population.end());

            //auto pool_size = population.size();

            for(auto i=0; i<pop_size/2 - 1; i+=2)
            {
                auto mate1 = population[i];
                auto mate2 = population[i+1];
                auto offsprings = Evolution::Crossover(mate1, mate2);
                population.push_back(std::get<0>(offsprings));
                population.push_back(std::get<1>(offsprings));
            }
            assert(population.size() == pop_size);

            std::cout << "best: " << best<< std::endl;
            std::cout << "elo difference: " << best.fitness << std::endl;
            std::cout << encoder.Decode(best);
            generation++;
        }
    }

    int Tuner::fitness(Chromosome string, Board& board, Encoder& encoder)
    {
        namespace bp = boost::process;
        std::string par_path = "parameters.txt";
        std::ofstream file(par_path);

        encoder.Decode(string);
        encoder.ApplyGenes(file);

        bp::ipstream is;
        bp::system("./fitness-tournament.sh " + par_path, bp::std_out > is);

        std::vector<std::string> data;
        std::string line;

        while(std::getline(is, line) && line.find("ELO")==std::string::npos) 
        {
            std::cout << line << std::endl;
        }

        int elo = -9999;

        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "));
        for (auto i=0u; i<tokens.size(); i++)
        {
            if (tokens[i] == "difference:")
            {
                try {
                    elo = std::stoi(tokens[i+1]);
                }
                catch(std::invalid_argument& e){
                    elo = -9999;
                }
            }
        }

        file.close();

        return elo;
        //auto result = bp::system("./fitness-tournament.sh " + par_path);

        /*
           std::string str = "";
           long long nodesVisited = 0;
           int correct = 0;

           int i = 0;
           for (auto fields : fenstrings)
           {
           Search::Table.Clear();
           std::cout << i++ << " ";

           FenString epd(fields[0]);
           board.LoadGame(epd.FullString);

           Move move;
           Search::depth_limit = 7;
           move = Search::StartThinking(SearchType::Infinite, board, false);
           nodesVisited += Search::searchInfo.Nodes();

           if(move.ToSan(board) == epd.BestMove)
           correct++;
           }
           std::cout << std::endl;

           std::cout << "nodes: " << nodesVisited << std::endl;
           double scaledNodes = nodesVisited/500000;
        //double scaledNodes = nodesVisited/(1 + nodesVisited);
        //double scaledCorrect = correct/(1 + correct);
        return std::make_pair(correct, correct - scaledNodes);
        //return std::make_pair(correct, scaledCorrect - scaledNodes);
        //return std::make_pair(correct, (correct - scaledNodes) - std::abs(scaledNodes - correct));
        //return std::make_pair(correct, (-nodesVisited + correct) - std::abs(-nodesVisited - correct));
        //return std::make_pair(correct, nodesVisited/(std::exp(correct/50)));
        //return std::exp(correct*3)/nodesVisited;
        */
    }

}

#ifndef ENCODER_H
#define ENCODER_H
#include "evolution.h"
#include "search.h"
#include <fstream>
#include <map>

using namespace Evolution;
namespace Napoleon
{
    class Encoder
    {
        public:
            Chromosome Encode(int arr[])
            {
                Chromosome encoded(Bits());
                int start = 0;
                for (auto const& m : sizes)
                {
                    Chromosome temp(m.second, arr[m.first]);
                    for(auto i=0u; i<temp.size(); i++)
                        encoded.set(start + i, temp.test(i));

                    start += m.second;
                }
                assert(encoded.size() == Bits());
                return encoded;
            }

            Encoder ()
                :sizes()
            {
            }

            size_t Bits()
            {
                return bits;
            }

            Encoder Decode(const Chromosome& code)
            {
                int start = 0;
                for(auto m : sizes)
                {
                    fields[m.first] = Subset(code, start, m.second).to_ulong();
                    start += m.second;
                }

                return *this;
            }

            void AddField(Search::Parameters id, size_t bits)
            {
                if (sizes.find(id) != sizes.end())
                    throw std::exception();

                sizes.emplace(std::make_pair(id, bits));
                //sizes[id] = bits;
                this->bits += bits;
            }

            int GetGene(Search::Parameters index)
            {
                return fields[index];
            }

            void ApplyGenes(ofstream& file)
            {
                for (auto const& m : sizes)
                    file << "setoption name " << Search::param_name[m.first] 
                        << " value " << fields[m.first] << std::endl;
            }

            friend ostream& operator<<(ostream& out, const Encoder& encoder)
            {
                out << "{ ";
                for (auto const& m : encoder.sizes)
                    out << encoder.fields[m.first] << " , ";
                out << " };" << std::endl;

                out << std::endl;
                for (auto const& m : encoder.sizes)
                    out << Search::param_name[m.first] << "=" << encoder.fields[m.first] << " ";

                //for (auto i=0; i<Search::Parameters::MAX; i++)
                //out << encoder.fields[i] << " , ";
                
                return out;

            }


        private:
            std::map<Search::Parameters, std::size_t> sizes;
            int fields[Search::Parameters::MAX];
            size_t bits = 0;
    };
}

#endif

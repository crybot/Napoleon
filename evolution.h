#ifndef EVOLUTION_H
#define EVOLUTION_H

#include <iostream>
#include <utility>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <boost/dynamic_bitset.hpp>

namespace Evolution
{
    using namespace std;
    static std::random_device rd;
    static std::default_random_engine gen(rd());
    typedef boost::dynamic_bitset<> d_bitset;

    class Chromosome : public d_bitset
    {
        public: 
            Chromosome() 
                :d_bitset()
            {
                reset_fitness();
            }
            Chromosome(size_t bits)
                :d_bitset(bits)
            {
                reset_fitness();
            }

            Chromosome(const string& str )
                :d_bitset(str)
            {
                reset_fitness();
            }

            Chromosome(size_t bits, unsigned long long val) 
                :d_bitset(bits, val)
            {
                reset_fitness();
            }

            d_bitset& set(size_t pos, bool value = true)
            {
                return d_bitset::set(d_bitset::size() - 1 - pos, value);
            }
            d_bitset& reset(size_t pos)
            {
                return d_bitset::reset(d_bitset::size() - 1 - pos);
            }
            d_bitset& flip(size_t pos)
            {
                return d_bitset::flip(d_bitset::size() - 1 - pos);
            }
            bool test(size_t pos) const
            {
                return d_bitset::test(d_bitset::size() - 1 - pos);
            }
            void reset_fitness()
            {
                fitness = -9999;
            }
            bool operator[]( std::size_t pos ) const = delete;
            typename d_bitset::reference operator[]( std::size_t pos ) = delete;

            int fitness;
        private:
            size_t bits;

    };

    static Chromosome Subset(Chromosome c, size_t start, size_t subsize)
    {
        Chromosome sub(subsize);
        for(auto i=start; i<(start+subsize); ++i)
            sub.set(i - start, c.test(i));

        return sub;
    }

    static Chromosome Mutate(Chromosome c)
    {
        std::bernoulli_distribution flip(3/(float)c.size());
        for (auto i=0u; i<c.size(); i++)
        {
            if (flip(gen))
                c.flip(i);
        }
        return c;
    }

    static pair<Chromosome, Chromosome> Crossover(Chromosome a, Chromosome b)
    {
        Chromosome first = a, second = b;
        for (auto i=(rand() % (a.size()-1)); i < a.size(); ++i)
        {
            first.set(i, b.test(i));
            second.set(i, a.test(i));
        }        
        first.reset_fitness();
        second.reset_fitness();
        return make_pair(Mutate(first), Mutate(second));
    }

    static Chromosome Random(size_t size)
    {
        std::bernoulli_distribution r(0.5);
        Chromosome random(size, 0);
        for (auto i=0u; i<random.size(); i++)
            random.set(i, r(gen));
        return random;
    }

};

#endif

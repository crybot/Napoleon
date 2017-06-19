#include "zobrist.h"
#include <random>

namespace Napoleon
{
    ZobristKey Zobrist::Piece[2][6][64];
    ZobristKey Zobrist::Castling[16];
    ZobristKey Zobrist::Enpassant[8];
    ZobristKey Zobrist::Color;

    class RandomGenerator
    {
    public:
        RandomGenerator() :gen(std::mt19937_64::default_seed) {   }

        unsigned long long Next()
        {
            return dist(gen);
        }

    private:
        std::uniform_int_distribution<unsigned long long> dist;
        std::mt19937_64 gen;
    };

    void Zobrist::Init()
    {
        RandomGenerator gen;

        for (int i=0; i< 2; i++)
        {
            for (int j=0; j<6; j++)
            {
                for (int k=0; k<64; k++)
                {
                    Piece[i][j][k] = gen.Next();
                }
            }
        }

        Color = gen.Next();

        for (int i=0; i<16; i++)
        {
            Castling[i] = gen.Next();
        }

        for (int i=0; i<8; i++)
        {
            Enpassant[i] = gen.Next();
        }
    }
}

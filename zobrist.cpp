#include "zobrist.h"

namespace Napoleon
{
    ZobristKey Zobrist::Piece[2][6][64];
    ZobristKey Zobrist::Castling[16];
    ZobristKey Zobrist::Enpassant[8];
    ZobristKey Zobrist::Color;

    ZobristKey Zobrist::random()
    {
        return rand() ^ ((ZobristKey)rand() << 15) ^ ((ZobristKey)rand() << 30) ^ ((ZobristKey)rand() << 45) ^ ((ZobristKey)rand() << 60);
    }

    void Zobrist::Init()
    {
        for (int i=0; i< 2; i++)
        {
            for (int j=0; j<6; j++)
            {
                for (int k=0; k<64; k++)
                {
                    Piece[i][j][k] = random();
                }
            }
        }

        Color = random();

        for (int i=0; i<16; i++)
        {
            Castling[i] = random();
        }

        for (int i=0; i<8; i++)
        {
            Enpassant[i] = random();
        }
    }
}

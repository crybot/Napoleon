#include "transpositiontable.h"
#include <ctime>
namespace Napoleon
{
    BitBoard Zobrist::Piece[2][6][64];
    BitBoard Zobrist::Castling[16];
    BitBoard Zobrist::Enpassant[8];
    BitBoard Zobrist::Color;


    TranspositionTable::TranspositionTable()
    {

    }

    BitBoard Zobrist::random()
    {
        return rand() ^ ((BitBoard)rand() << 15) ^ ((BitBoard)rand() << 30) ^ ((BitBoard)rand() << 45) ^ ((BitBoard)rand() << 60);
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

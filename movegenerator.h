#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H
#include "defines.h"

namespace Napoleon
{
    class Move;
    class Board;
    class MoveGenerator
    {
    public:
        static void GetAllMoves(Move* allMoves[], int& pos, Board& board);
        static void GetPawnMoves(BitBoard pawns, Board& board, Move* moveList[], int& pos);
        static void GetKingMoves(BitBoard king, Board& board, Move* moveList[], int& pos);
        static void GetKnightMoves(BitBoard knights, Board& board, Move* moveList[], int& pos);
        static void GetRookMoves(BitBoard rooks, Board& board, Move* moveList[], int& pos);
        static void GetBishopMoves(BitBoard bishops, Board& board, Move* moveList[],int& pos);
        static void GetQueenMoves(BitBoard queens, Board& board, Move* moveList[], int& pos);
        static void GetCastleMoves(Board board, Move* moveList[], int& pos);
    };
}
#endif // MOVEGENERATOR_H

#include <iostream>
#include "utils.h"
#include "constants.h"
#include "stopwatch.h"
#include "piece.h"
#include "compassrose.h"
#include "board.h"
#include "pawn.h"
#include "knight.h"
#include "king.h"
#include "rook.h"
#include "bishop.h"
#include "queen.h"
#include "movedatabase.h"
#include "fenstring.h"
#include "movegenerator.h"
#include <cstdio>
#include <ctime>

using namespace Napoleon;
using namespace std;

typedef unsigned long long uint64;
const uint64 m1=0x5555555555555555;
const uint64 m2=0x3333333333333333;
const uint64 m4=0x0f0f0f0f0f0f0f0f;
const uint64 m8=0x00ff00ff00ff00ff;
const uint64 m16=0x0000ffff0000ffff;
const uint64 m32=0x00000000ffffffff;
const uint64 hff=0xffffffffffffffff;
const uint64 h01=0x0101010101010101;

BitBoard Random()
{
    BitBoard r30 = RAND_MAX*rand()+rand();
    BitBoard s30 = RAND_MAX*rand()+rand();
    BitBoard t4  = rand() & 0xf;

    BitBoard res = (r30 << 34) + (s30 << 4) + t4;

    return res;
}

void Bench(int (*f)(const BitBoard))
{

    BitBoard x = Random();
    StopWatch watch;
    watch.Start();

    for(unsigned long it=0; it<=(1ul<<29); ++it)
    {
        f(x);
    }

    cout << watch.Stop().ElapsedMilliseconds() << " ms" << endl;
}

void Bench(int (*f)(BitBoard, Board& ))
{

    Board board;
    board.Equip();
    StopWatch watch;
    watch.Start();

    for(unsigned long it=0; it<=(1ul<<27); ++it)
    {
        f(board.GetPieceSet(PieceColor::White, PieceType::Queen), board);
    }

    cout << watch.Stop().ElapsedMilliseconds() << " ms" << endl;
}

int test(BitBoard b, Board& c)
{
    Queen::GetAllTargets(b, c);
    return 0;
}

int Perft(int depth, Board& board)
{
    int pos = 0;
    Move moves[Constants::MaxMoves + 2];

    int nodes = 0;

    if (depth == 0)
        return 1;

    MoveGenerator::GetLegalMoves(moves, pos, board);

    if (depth == 1)
        return pos;

    for (int i = 0; i < pos; i++)
    {
        board.MakeMove(moves[i]);
        nodes += Perft(depth - 1, board);
        board.UndoMove(moves[i]);
    }

    return nodes;

}

void Divide(int depth, Board& board)
{
    int pos = 0;
    int total = 0;
    int temp = 0;
    int NumMoves = 0;

    Move moves[Constants::MaxMoves + 2];
    MoveGenerator::GetLegalMoves(moves, pos, board);

    std::cout << "Move\tNodes" << std::endl;

    for (int i = 0; i < pos; i++)
    {
        board.MakeMove(moves[i]);
        temp = Perft(depth - 1, board);
        total += temp;
        std::cout << moves[i].ToAlgebraic() << "\t" << temp << std::endl;
        board.UndoMove(moves[i]);
        NumMoves++;
    }

    std::cout << "Total Nodes: " << total << std::endl;
    std::cout << "Moves: " << NumMoves << std::endl;

}

int main()
{
    using namespace Constants;
    using namespace Constants::Squares;
    srand(time(NULL));
    Board board;
    board.LoadGame(FenString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));

    //    Move moves[100];
    //    int pos;

    StopWatch watch;
    watch.Start();

    //    board.Display();
    //    cin.get();

    //    board.MakeMove(Move(IntE2, IntE4, PieceType::Pawn, PieceType::None, PieceType::None));
    //    board.Display();
    //    cin.get();

    //    board.MakeMove(Move(IntA7, IntA6, PieceType::Pawn, PieceType::None, PieceType::None));
    //    board.Display();
    //    cin.get();

    //    board.MakeMove(Move(IntE4, IntE5, PieceType::Pawn, PieceType::None, PieceType::None));
    //    board.Display();
    //    cin.get();

    //    board.MakeMove(Move(IntD7, IntD5, PieceType::Pawn, PieceType::None, PieceType::None));
    //    board.Display();
    //    cin.get();

    //    board.MakeMove(Move(IntE5, IntD6, PieceType::Pawn, PieceType::Pawn, PieceType::Pawn));
    //    board.Display();
    //    cin.get();

    //    board.UndoMove(Move(IntE5, IntD6, PieceType::Pawn, PieceType::Pawn, PieceType::Pawn));
    //    board.Display();
    //    cin.get();

    //    cout << "BitBoard dopo Enpassant: " << endl;
    //    cin.get();
    //    cout << "Pezzi Bianchi : " << endl;
    //    Utils::BitBoard::Display(board.WhitePieces);
    //    cin.get();

    //    cout << "Pezzi Neri : " << endl;
    //    Utils::BitBoard::Display(board.BlackPieces);
    //    cin.get();

    //    cout << "Caselle occupate : " << endl;
    //    Utils::BitBoard::Display(board.OccupiedSquares);
    //    cin.get();

    //    cout << "Caselle libere : " << endl;
    //    Utils::BitBoard::Display(board.EmptySquares);
    //    cin.get();

    //    cout << "Pedoni bianchi: " << endl;
    //    Utils::BitBoard::Display(board.GetPieceSet(PieceColor::White, PieceType::Pawn));
    //    cin.get();

    //    cout << "Pedoni neri: " << endl;
    //    Utils::BitBoard::Display(board.GetPieceSet(PieceColor::Black, PieceType::Pawn));
    //    cin.get();




    Divide(1, board);
    watch.Stop();
    cout << "Tempo Impiegato (ms): " << watch.ElapsedMilliseconds() << endl;

    return 0;
}

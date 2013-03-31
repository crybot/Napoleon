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

int main()
{
    srand(time(NULL));
    Board board;
    board.LoadGame(FenString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    Move* moves[100];
    int pos = 0;
    MoveGenerator::GetAllMoves(moves, pos, board);

    StopWatch watch;
    watch.Start();

    for (int i=0; i<pos; i++)
    {
        string s = moves[i]->ToAlgebraic();
        cout << s << endl;
    }

    watch.Stop();

    cout << "Tempo Impiegato: " << watch.ElapsedMilliseconds() << endl;


    //    cout << "Tempo Impiegato: "; Bench(test);




    return 0;
}

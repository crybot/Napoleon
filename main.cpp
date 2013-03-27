#include <iostream>
#include "utils.h"
#include "constants.h"
#include "stopwatch.h"
#include "piece.h"
#include "compassrose.h"
#include "board.h"
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

void Bench(int (*f)(BitBoard))
{

    BitBoard x = Random();
    StopWatch watch;
    watch.Start();

    for(unsigned long it=0; it<=(1ul<<26); ++it)
    {
        f(x);
    }

    cout << watch.Stop().ElapsedMilliseconds() << " ms" << endl;
}

int popcount1(BitBoard bitBoard)
{
    Piece piece(1,1);
    return 0;
}

int popcount2(BitBoard x)
{
    x-=(x>>1)&m1;//put count of each 2 bits into those 2 bits
    x=(x&m2)+((x>>2)&m2);//put count of each 4 bits into those 4 bits
    x=(x+(x>>4))&m4; //put count of each 8 bits into those 8 bits
    x+=x>>8;//put count of each 16 bits into their lowest 8 bits
    x+=x>>16;
    x+=x>>32;
    return x&0x7f;
}

int popcount3(BitBoard x)
{
    BitBoard  count;
    for(count=0; x; count++)
    {
        x&=x-1;
    }
    return count;
}

int main()
{
    srand(time(NULL));
    Board board;
    board.Equip();
    board.Display();


    cin.get();


    return 0;
}

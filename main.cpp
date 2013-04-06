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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
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

unsigned long long Perft(int depth, Board& board)
{
    int pos = 0;
    Move moves[Constants::MaxMoves + 2];

    unsigned long long nodes = 0;

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
    unsigned long long total = 0;
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

//bool IsEnPassant(Byte a, Byte b, Byte c)
//{
//    return (a == PieceType::Pawn && b == PieceType::Pawn && c == PieceType::Pawn);
//}

int main()
{
    using namespace Constants;
    using namespace Constants::Squares;


    //    int IntMove = 0;

    //    IntMove |= (Constants::Squares::IntH1 & 0x3f); // from
    //    IntMove |= (Constants::Squares::IntA2 & 0x3f) << 6; // to
    //    IntMove |= (PieceType::Knight & 0x7) << 12; // piece moved
    //    IntMove |= (PieceType::Pawn & 0x7) << 15; // piece captured
    //    IntMove |= (PieceType::Pawn & 0x7) << 18; // piece promoted

    //    Byte pm = (IntMove >> 12) & 0x7;
    //    Byte pc = (IntMove >> 15) & 0x7;
    //    Byte prm = (IntMove >> 18) & 0x7;

    //    if (IsEnPassant(pm, pc, prm))
    //        cout << "En passant!";

    //    cin.get();


    StopWatch watch;
    Board board;
    board.LoadGame(FenString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1"));

    while(1)
    {
        cout << "Napoleon: ";
        vector<string> fields;
        string cmd;
        std::getline(cin, cmd);

        boost::split(fields, cmd, boost::is_any_of(" "));

        if (fields[0] == "perft")
        {
            if (fields.size() > 1)
            {
                int depth = boost::lexical_cast<int>(fields[1]);
                watch.Start();
                cout << "Perft(" << depth << "): ";
                cout << "Total Nodes: " << Perft(depth, board) << endl;
                cout << "Time (ms): " << watch.Stop().ElapsedMilliseconds() << endl;
                //                assert(initialBoard.WhitePieces ==  board.WhitePieces);
                //                assert(initialBoard.BlackPieces ==  board.BlackPieces);
                //                assert(initialBoard.EmptySquares ==  board.EmptySquares);
                //                assert(initialBoard.OccupiedSquares ==  board.OccupiedSquares);
                //                assert(initialBoard.EnPassantSquare ==  board.EnPassantSquare);
            }
        }

        else if (fields[0] == "divide")
        {
            if (fields.size() > 1)
            {
                watch.Start();
                Divide(boost::lexical_cast<int>(fields[1]), board);
                cout << "Time (ms): " << watch.Stop().ElapsedMilliseconds() << endl;
                //                assert(initialBoard.WhitePieces ==  board.WhitePieces);
                //                assert(initialBoard.BlackPieces ==  board.BlackPieces);
                //                assert(initialBoard.EmptySquares ==  board.EmptySquares);
                //                assert(initialBoard.OccupiedSquares ==  board.OccupiedSquares);
                //                assert(initialBoard.EnPassantSquare ==  board.EnPassantSquare);
            }
        }


        else if (fields[0] == "set")
        {
            if (fields.size() >= 5)
            {
                board.LoadGame(FenString(cmd.substr(fields[0].size()+1)));
            }
        }

        else if (cmd == "display")
        {
            board.Display();
        }

        else if (cmd == "debug")
        {
            cout << "Pezzi bianchi: "; Utils::BitBoard::Display(board.Pieces[PieceColor::White]);
            cin.get();
            cout << "Pezzi Neri: "; Utils::BitBoard::Display(board.Pieces[PieceColor::Black]);
            cin.get();
            cout << "Caselle occupate: "; Utils::BitBoard::Display(board.OccupiedSquares);
            cin.get();
            cout << "Caselle libere: "; Utils::BitBoard::Display(board.EmptySquares);
            cin.get();
        }

        else if (cmd == "new")
        {
            board = Board();
            board.Equip();
        }

        else if (cmd == "pinned")
        {
            cout << "Pinned Pieces: " << endl;
            Utils::BitBoard::Display(board.GetPinnedPieces());
        }

        else if (fields[0] == "move")
        {
            if (fields.size() == 2)
            {
                board.MakeMove(board.ParseMove(fields[1]));
            }
        }

        else if (fields[0] == "undo")
        {
            if (fields.size() == 2)
            {
                board.UndoMove(board.ParseMove(fields[1]));
            }
        }

        else
        {
            cout << "incorrect command: " << cmd << endl;
        }
        cout << endl;
    }

    return 0;
}

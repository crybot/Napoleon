#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "move.h"
#include "piece.h"
#include "defines.h"
#include <limits>

namespace Napoleon
{
    namespace Constants
    {

        namespace Magics
        {
            const BitBoard FileMagic[] =
            {
                0x2040810214010, 0x1020408902000, 0x900810204081000, 0x40a102040800, 
                0xc02040810a0404, 0x4000102040890200, 0x803c081020408100, 0x40810204280, 
            };


            const BitBoard A1H8DiagonalMagic[] =
            {
                0x80300c00021660, 0xc0000008040020, 0x8080090880400088, 0x4004141020080841, 
                0x202820800080, 0x2000404040411020, 0xa0808890808050, 0x801010111090103, 
                0x4010101010904, 0x2028181010900, 0x201048001012100, 0x40880a4020018111, 
                0x8480025400024101, 0x2498114011411000, 0x85040401c6404040, 
            };

            const BitBoard H1A8DiagonalMagic[] =
            {
                0x80300c00021660, 0xc0000008040020, 0x8001004020010846, 0x820904004002, 
                0x410460024080, 0x4080208208000040, 0x200104104100406, 0x220082082082020, 
                0x40420828820800, 0x2a1200008208225, 0x8480018100082080, 0x4008cf1028000820, 
                0x4b10108000001008, 0x420000010802000, 0x808820001, 
            };

        }

        namespace Squares
        {
            enum Square : Napoleon::Square
            {
                IntA1, IntB1, IntC1, IntD1, IntE1, IntF1, IntG1, IntH1,
                IntA2, IntB2, IntC2, IntD2, IntE2, IntF2, IntG2, IntH2,
                IntA3, IntB3, IntC3, IntD3, IntE3, IntF3, IntG3, IntH3,
                IntA4, IntB4, IntC4, IntD4, IntE4, IntF4, IntG4, IntH4,
                IntA5, IntB5, IntC5, IntD5, IntE5, IntF5, IntG5, IntH5,
                IntA6, IntB6, IntC6, IntD6, IntE6, IntF6, IntG6, IntH6,
                IntA7, IntB7, IntC7, IntD7, IntE7, IntF7, IntG7, IntH7,
                IntA8, IntB8, IntC8, IntD8, IntE8, IntF8, IntG8, IntH8
            };

            const Napoleon::Square Invalid = 65;
            const BitBoard A1 = 0x0000000000000001;
            const BitBoard B1 = 0x0000000000000002;
            const BitBoard C1 = 0x0000000000000004;
            const BitBoard D1 = 0x0000000000000008;
            const BitBoard E1 = 0x0000000000000010;
            const BitBoard F1 = 0x0000000000000020;
            const BitBoard G1 = 0x0000000000000040;
            const BitBoard H1 = 0x0000000000000080;

            const BitBoard A2 = 0x0000000000000100;
            const BitBoard B2 = 0x0000000000000200;
            const BitBoard C2 = 0x0000000000000400;
            const BitBoard D2 = 0x0000000000000800;
            const BitBoard E2 = 0x0000000000001000;
            const BitBoard F2 = 0x0000000000002000;
            const BitBoard G2 = 0x0000000000004000;
            const BitBoard H2 = 0x0000000000008000;

            const BitBoard A3 = 0x0000000000010000;
            const BitBoard B3 = 0x0000000000020000;
            const BitBoard C3 = 0x0000000000040000;
            const BitBoard D3 = 0x0000000000080000;
            const BitBoard E3 = 0x0000000000100000;
            const BitBoard F3 = 0x0000000000200000;
            const BitBoard G3 = 0x0000000000400000;
            const BitBoard H3 = 0x0000000000800000;

            const BitBoard A4 = 0x0000000001000000;
            const BitBoard B4 = 0x0000000002000000;
            const BitBoard C4 = 0x0000000004000000;
            const BitBoard D4 = 0x0000000008000000;
            const BitBoard E4 = 0x0000000010000000;
            const BitBoard F4 = 0x0000000020000000;
            const BitBoard G4 = 0x0000000040000000;
            const BitBoard H4 = 0x0000000080000000;

            const BitBoard A5 = 0x0000000100000000;
            const BitBoard B5 = 0x0000000200000000;
            const BitBoard C5 = 0x0000000400000000;
            const BitBoard D5 = 0x0000000800000000;
            const BitBoard E5 = 0x0000001000000000;
            const BitBoard F5 = 0x0000002000000000;
            const BitBoard G5 = 0x0000004000000000;
            const BitBoard H5 = 0x0000008000000000;

            const BitBoard A6 = 0x0000010000000000;
            const BitBoard B6 = 0x0000020000000000;
            const BitBoard C6 = 0x0000040000000000;
            const BitBoard D6 = 0x0000080000000000;
            const BitBoard E6 = 0x0000100000000000;
            const BitBoard F6 = 0x0000200000000000;
            const BitBoard G6 = 0x0000400000000000;
            const BitBoard H6 = 0x0000800000000000;

            const BitBoard A7 = 0x0001000000000000;
            const BitBoard B7 = 0x0002000000000000;
            const BitBoard C7 = 0x0004000000000000;
            const BitBoard D7 = 0x0008000000000000;
            const BitBoard E7 = 0x0010000000000000;
            const BitBoard F7 = 0x0020000000000000;
            const BitBoard G7 = 0x0040000000000000;
            const BitBoard H7 = 0x0080000000000000;

            const BitBoard A8 = 0x0100000000000000;
            const BitBoard B8 = 0x0200000000000000;
            const BitBoard C8 = 0x0400000000000000;
            const BitBoard D8 = 0x0800000000000000;
            const BitBoard E8 = 0x1000000000000000;
            const BitBoard F8 = 0x2000000000000000;
            const BitBoard G8 = 0x4000000000000000;
            const BitBoard H8 = 0x8000000000000000;
        }

        namespace Ranks
        {
            enum Rank : Napoleon::Rank
            {
                Int1=0, Int2, Int3, Int4, Int5, Int6, Int7, Int8
            };

            const BitBoard One = 0x00000000000000FF;
            const BitBoard Two = 0x000000000000FF00;
            const BitBoard Three = 0x0000000000FF0000;
            const BitBoard Four = 0x00000000FF000000;
            const BitBoard Five = 0x000000FF00000000;
            const BitBoard Six = 0x0000FF0000000000;
            const BitBoard Seven = 0x00FF000000000000;
            const BitBoard Eight = 0xFF00000000000000;
        }

        namespace Files
        {
            enum File : Napoleon::File
            {
                IntA=0, IntB, IntC, IntD, IntE, IntF, IntG, IntH
            };

            const BitBoard A = 0x0101010101010101;
            const BitBoard B = 0x0202020202020202;
            const BitBoard C = 0x0404040404040404;
            const BitBoard D = 0x0808080808080808;
            const BitBoard E = 0x1010101010101010;
            const BitBoard F = 0x2020202020202020;
            const BitBoard G = 0x4040404040404040;
            const BitBoard H = 0x8080808080808080;
        }

        namespace InitialPositions
        {
            const BitBoard WhitePawns = 0x000000000000FF00;
            const BitBoard WhiteKnights = 0x0000000000000042;
            const BitBoard WhiteBishops = 0x0000000000000024;
            const BitBoard WhiteRooks = 0x0000000000000081;
            const BitBoard WhiteQueen = 0x0000000000000008;
            const BitBoard WhiteKing = 0x0000000000000010;

            const BitBoard BlackPawns = 0x00FF000000000000;
            const BitBoard BlackKnights = 0x4200000000000000;
            const BitBoard BlackBishops = 0x2400000000000000;
            const BitBoard BlackRooks = 0x8100000000000000;
            const BitBoard BlackQueen = 0x0800000000000000;
            const BitBoard BlackKing = 0x1000000000000000;
        }

        namespace Masks
        {
            /// <summary>
            /// questi array con esattamente 6 bit attivi per ciascun elemento
            /// saranno utili nella generazione delle mosse delle torri e delle regine
            /// per ottenere i gradi di occupazione a 6 bit di tali pezzi
            /// </summary>

            const BitBoard SquareMask[] =
            {
                Squares::A1, Squares::B1, Squares::C1, Squares::D1, Squares::E1, Squares::F1, Squares::G1, Squares::H1,
                Squares::A2, Squares::B2, Squares::C2, Squares::D2, Squares::E2, Squares::F2, Squares::G2, Squares::H2,
                Squares::A3, Squares::B3, Squares::C3, Squares::D3, Squares::E3, Squares::F3, Squares::G3, Squares::H3,
                Squares::A4, Squares::B4, Squares::C4, Squares::D4, Squares::E4, Squares::F4, Squares::G4, Squares::H4,
                Squares::A5, Squares::B5, Squares::C5, Squares::D5, Squares::E5, Squares::F5, Squares::G5, Squares::H5,
                Squares::A6, Squares::B6, Squares::C6, Squares::D6, Squares::E6, Squares::F6, Squares::G6, Squares::H6,
                Squares::A7, Squares::B7, Squares::C7, Squares::D7, Squares::E7, Squares::F7, Squares::G7, Squares::H7,
                Squares::A8, Squares::B8, Squares::C8, Squares::D8, Squares::E8, Squares::F8, Squares::G8, Squares::H8,
            };

            const BitBoard SixBitRankMask[] =
            {
                0x000000000000007E, 0x0000000000007E00, 0x00000000007E0000,
                0x000000007E000000, 0x0000007E00000000, 0x00007E0000000000,
                0x007E000000000000, 0x7E00000000000000
            };

            const BitBoard SixBitFileMask[] =
            {
                0x0001010101010100, 0x0002020202020200, 0x0004040404040400,
                0x0008080808080800, 0x0010101010101000, 0x0020202020202000,
                0x0040404040404000, 0x0080808080808000
            };

            const BitBoard RankMask[] =
            {
                0x000000000000007E, 0x0000000000007E00, 0x00000000007E0000,
                0x000000007E000000, 0x0000007E00000000, 0x00007E0000000000,
                0x007E000000000000, 0x7E00000000000000
            };

            const BitBoard FileMask[] =
            {
                0x101010101010101, 0x202020202020202, 0x404040404040404,
                0x808080808080808, 0x1010101010101010, 0x2020202020202020,
                0x4040404040404040, 0x8080808080808080
            };

            const BitBoard A1H8DiagonalMask[] =
            {
                0x0000000000000080, 0x0000000000008040, 0x0000000000804020, 0x0000000080402010,
                0x0000008040201008, 0x0000804020100804, 0x0080402010080402, 0x8040201008040201,
                0x4020100804020100, 0x2010080402010000, 0x1008040201000000, 0x0804020100000000,
                0x0402010000000000, 0x0201000000000000, 0x0100000000000000
            };

            const BitBoard SixBitA1H8DiagonalMask[] =
            {
                0x0, 0x0, 0x4000, 0x402000,
                0x40201000, 0x4020100800, 0x402010080400, 0x40201008040200,
                0x20100804020000, 0x10080402000000, 0x8040200000000,
                0x4020000000000, 0x2000000000000, 0x0, 0x0
            };

            const BitBoard H1A8DiagonalMask[] =
            {
                0x0000000000000001, 0x0000000000000102, 0x0000000000010204, 0x0000000001020408,
                0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080,
                0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000,
                0x2040800000000000, 0x4080000000000000, 0x8000000000000000
            };

            const BitBoard SixBitH1A8DiagonalMask[] = 
            {
                0x0, 0x0, 0x200, 0x20400, 0x2040800, 0x204081000, 0x20408102000, 
                0x2040810204000, 0x4081020400000, 0x8102040000000, 0x10204000000000,
                0x20400000000000, 0x40000000000000, 0x0, 0x0
            };
        }

        namespace Castle
        {
            const Byte WhiteCastleOO = 0x1;
            const Byte WhiteCastleOOO = 0x2;
            const Byte BlackCastleOO = 0x4;
            const Byte BlackCastleOOO = 0x8;
            const Byte FullCastlingRights = 0xF;

            const BitBoard WhiteCastleMaskOO = 0x0000000000000060;
            const BitBoard WhiteCastleMaskOOO = 0x000000000000000E;
            const BitBoard BlackCastleMaskOO = 0x6000000000000000;
            const BitBoard BlackCastleMaskOOO = 0x0E00000000000000;

            const BitBoard WhiteKingShield = 0xe000; // 3-pawn shield (f-g-h)
            const BitBoard WhiteQueenShield = 0x700; // 3-pawn shield (a-b-c)
            const BitBoard BlackKingShield = 0xe0000000000000; 
            const BitBoard BlackQueenShield = 0x7000000000000;

            const BitBoard WhiteKingSide = 0xe0;
            const BitBoard WhiteQueenSide = 0x7;
            const BitBoard BlackKingSide = 0xe000000000000000;
            const BitBoard BlackQueenSide = 0x700000000000000;

            const Move WhiteCastlingOO(Squares::IntE1, Squares::IntG1, KingCastle);
            const Move WhiteCastlingOOO(Squares::IntE1, Squares::IntC1, QueenCastle);

            const Move BlackCastlingOO(Squares::IntE8, Squares::IntG8, KingCastle);
            const Move BlackCastlingOOO(Squares::IntE8, Squares::IntC8, QueenCastle);
        }

        namespace Piece
        {
            const Napoleon::Piece Null = Napoleon::Piece(PieceColor::None, PieceType::None);
            const int PieceValue[] = { 100, 330, 330, 500, 1000, 2000, 99999 }; // Null-Piece = +Inf
        }

        namespace Eval
        {
            using namespace Constants::Piece;
            // Side to move relative
            const int OpeningGameMat = PieceValue[PieceType::Pawn]*16
                + PieceValue[PieceType::Knight]*4
                + PieceValue[PieceType::Bishop]*4
                + PieceValue[PieceType::Rook]*4
                + PieceValue[PieceType::Queen]*2
                + PieceValue[PieceType::King]*2;

            const int MaxPlayerMat = OpeningGameMat/2;

            const int MiddleGameMat = OpeningGameMat - 1200;
            const int EndGameMat = MiddleGameMat - 3000;

            const int OpeningNonPawnMaterial = OpeningGameMat -  PieceValue[PieceType::Pawn]*16;

            const int BishopPair[3] = { 30, 50, 50 };
            const int PawnBonus[3] = { 0, 10, 30 };
            const int KnightBonus[3] = { 3, -5, -12 };
            const int RookBonus[3] = { 0, 5, 15 };
            const int PasserBonus[3] = {20, 30, 40};
            const int HalfOpenFileBonus[3] = {5, 8, 0}; // OpenFileBonus = HalfOpenFileBonus*2

            const int MaxPhase = 256;
            //            const int MaxNonPawnMaterial = PieceValue[PieceType::Knight]*4;
        }

        const std::string StartPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        const int CaptureMask = 0x4;
        const int EpMask = 0x5;

        const Move NullMove(0, 0);
        const BitBoard Empty = 0x0000000000000000;
        const BitBoard Universe = 0xFFFFFFFFFFFFFFFF;

        const BitBoard LightSquares = 0x55AA55AA55AA55AA;
        const BitBoard DarkSquares = 0xAA55AA55AA55AA55;

        const BitBoard NotAFile = ~Files::A;
        const BitBoard NotBFile = ~Files::B;
        const BitBoard NotCFile = ~Files::C;
        const BitBoard NotDFile = ~Files::D;
        const BitBoard NotEFile = ~Files::E;
        const BitBoard NotFFile = ~Files::F;
        const BitBoard NotGFile = ~Files::G;
        const BitBoard NotHFile = ~Files::H;
        const BitBoard NotABFile = NotAFile | NotBFile;
        const BitBoard NotGHFile = NotGFile | NotHFile;

        const int Infinity = 200000;
        const int Unknown = 2*Infinity;
        const int Mate = std::numeric_limits<short>::max();
        const int MaxMoves = 192;
        const int MaxPly = 1024;
        const int QueenMaxMoves = 27;

        const BitBoard DeBrujinValue = 0x07EDD5E59A4E28C2;
        const int DeBrujinTable[] =
        {
            63,  0, 58,  1, 59, 47, 53,  2,
            60, 39, 48, 27, 54, 33, 42,  3,
            61, 51, 37, 40, 49, 18, 28, 20,
            55, 30, 34, 11, 43, 14, 22,  4,
            62, 57, 46, 52, 38, 26, 32, 41,
            50, 36, 17, 19, 29, 10, 13, 21,
            56, 45, 25, 31, 35, 16,  9, 12,
            44, 24, 15,  8, 23,  7,  6,  5
        };

    }
}

#endif // CONSTANTS_H

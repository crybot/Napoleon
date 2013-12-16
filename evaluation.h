#ifndef EVALUATION_H
#define EVALUATION_H
#include "defines.h"
namespace Napoleon
{
    class Board;
    class Piece;
    class Evaluation
    {
    public:
        static int Evaluate(Board&);
        static int EvaluatePiece(Piece, Square, Board&);
        static int CalculatePST(Piece, Square, Board&);

    private:
        template<Byte>
        static int evaluateMobility(Board&, BitBoard);

        static int pawnSquareValue[64];
        static int knightSquareValue[64];
        static int bishopSquareValue[64];
        static int rookSquareValue[64];
        static int queenSquareValue[64];
        static int kingMiddleGame[64];
		static int kingEndGame[64];

        static int multiPawnP[8]; // penalization for doubled, tripled... pawns


    };

}

#endif // EVALUATION_H

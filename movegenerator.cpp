#include "movegenerator.h"
#include "queen.h"

namespace Napoleon
{    
    void MoveGenerator::GetQueenMoves(BitBoard queens, Board &board, Move moveList[], int &pos)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (queens != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(queens); // search for LS1B and then reset it
            targets = Queen::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board);

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Queen, board.pieceSet[toIndex].Type, PieceType::None);
            }
        }
    }
}

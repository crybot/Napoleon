#include "movegenerator.h"
#include "queen.h"

namespace Napoleon
{
    void MoveGenerator::GetQueenMoves(BitBoard queens, Board &board, Move moveList[], int &pos, BitBoard target)
    {
        BitBoard targets;
        int fromIndex;
        int toIndex;

        while (queens != 0)
        {
            fromIndex = Utils::BitBoard::BitScanForwardReset(queens); // search for LS1B and then reset it
            targets = Queen::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex);
            }
        }
    }
}

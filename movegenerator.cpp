#include "movegenerator.h"
#include "queen.h"

namespace Napoleon
{
    void MoveGenerator::GetQueenMoves(BitBoard queens, Board &board, Move moveList[], int &pos, BitBoard target)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (queens != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(queens); // search for LS1B and then reset it
            targets = Queen::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    //thanks stockfish for this
    void MoveGenerator::GetEvadeMoves(Board& board, BitBoard checkers, Move moveList[], int& pos)
    {
        BitBoard b;
        Byte to;
        int from, checksq;
        int checkersCnt = 0;
        int ksq = board.KingSquare[board.SideToMove];
        BitBoard sliderAttacks = 0;

        // Find squares attacked by slider checkers, we will remove them from the king
        // evasions so to skip known illegal moves avoiding useless legality check later.
        b = checkers;
        do
        {
            checkersCnt++;
            checksq = Utils::BitBoard::BitScanForwardReset(b);

            switch (board.PieceSet[checksq].Type)
            {
            case PieceType::Bishop: sliderAttacks |= MoveDatabase::PseudoBishopAttacks[checksq]; break;
            case PieceType::Rook: sliderAttacks |= MoveDatabase::PseudoRookAttacks[checksq]; break;
            case PieceType::Queen:
                // If queen and king are far or not on a diagonal line we can safely
                // remove all the squares attacked in the other direction becuase are
                // not reachable by the king anyway.
                if ( MoveDatabase::ObstructedTable[ksq][checksq] || (MoveDatabase::PseudoBishopAttacks[checksq] & Constants::Masks::SquareMask[ksq])==0)
                    sliderAttacks |=  MoveDatabase::PseudoBishopAttacks[checksq]
                            | MoveDatabase::PseudoRookAttacks[checksq];

                // Otherwise we need to use real rook attacks to check if king is safe
                // to move in the other direction. For example: king in B2, queen in A1
                // a knight in B1, and we can safely move to C1.
                else
                    sliderAttacks |= MoveDatabase::PseudoBishopAttacks[checksq] | (MoveDatabase::GetRankAttacks(board.OccupiedSquares, checksq)
                                                                                   | MoveDatabase::GetFileAttacks(board.OccupiedSquares, checksq));
                break;

            default:
                break;
            }
        } while (b);

        // Generate evasions for king, capture and non capture moves
        b = MoveDatabase::KingAttacks[ksq] & ~board.GetPlayerPieces() & ~sliderAttacks;
        from = ksq;

        while (b)
        {
            to = (Byte)Utils::BitBoard::BitScanForwardReset(b); // search for LS1B and then reset it
            moveList[pos++] = Move(from, to, board.PieceSet[to].Type, PieceType::None);
        }

        // Generate evasions for other pieces only if not under a double check
        if (checkersCnt > 1)
            return;

        // Blocking evasions or captures of the checking piece
        BitBoard target = MoveDatabase::ObstructedTable[checksq][ksq] | checkers;
        GetPawnMoves(board.GetPieceSet(board.SideToMove, PieceType::Pawn), board, moveList, pos, target);
        GetKnightMoves(board.GetPieceSet(board.SideToMove, PieceType::Knight), board, moveList, pos, target);
        GetBishopMoves(board.GetPieceSet(board.SideToMove, PieceType::Bishop), board, moveList, pos, target);
        GetRookMoves(board.GetPieceSet(board.SideToMove, PieceType::Rook), board, moveList, pos, target);
        GetQueenMoves(board.GetPieceSet(board.SideToMove, PieceType::Queen), board, moveList, pos, target);
    }

}

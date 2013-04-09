#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H
#include "defines.h"
#include "board.h"
#include "move.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "king.h"
#include "constants.h"
#include "piece.h"
#include "movedatabase.h"
#include "utils.h"

namespace Napoleon
{

    namespace MoveGenerator
    {
        void GetLegalMoves(Move allMoves[],int& pos, Board& board);
        void GetAllMoves(Move allMoves[], int& pos, Board& board, BitBoard target = Constants::Universe);
        void GetPawnMoves(BitBoard pawns, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetKingMoves(BitBoard king, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetKnightMoves(BitBoard knights, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetRookMoves(BitBoard rooks, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetBishopMoves(BitBoard bishops, Board& board, Move moveList[],int& pos, BitBoard target);
        void GetQueenMoves(BitBoard queens, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetCastleMoves(Board& board, Move moveList[], int& pos);
        void GetEvadeMoves(Board& board, BitBoard attackers, Move moveList[], int& pos);
    }

    INLINE void MoveGenerator::GetAllMoves(Move allMoves[], int& pos, Board& board, BitBoard target)
    {
        GetPawnMoves(board.GetPieceSet(board.SideToMove, PieceType::Pawn), board, allMoves, pos, target);
        GetKnightMoves(board.GetPieceSet(board.SideToMove, PieceType::Knight), board, allMoves, pos, target);
        GetKingMoves(board.GetPieceSet(board.SideToMove, PieceType::King), board, allMoves, pos, target);
        GetBishopMoves(board.GetPieceSet(board.SideToMove, PieceType::Bishop), board, allMoves, pos, target);
        GetRookMoves(board.GetPieceSet(board.SideToMove, PieceType::Rook), board, allMoves, pos, target);
        GetQueenMoves(board.GetPieceSet(board.SideToMove, PieceType::Queen), board, allMoves, pos, target);
        GetCastleMoves(board, allMoves, pos);
    }

    INLINE void MoveGenerator::GetPawnMoves(BitBoard pawns, Board& board, Move moveList[], int& pos, BitBoard target)
    {
        BitBoard targets;
        BitBoard epTargets;
        int fromIndex;
        int toIndex;

        while (pawns != 0)
        {
            fromIndex = Utils::BitBoard::BitScanForwardReset(pawns); // search for LS1B and then reset it
            targets = Pawn::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            // en passant
            if (board.EnPassantSquare != Constants::Squares::Invalid)
            {
                epTargets = MoveDatabase::PawnAttacks[board.SideToMove][fromIndex];

                if ((epTargets & Constants::Masks::SquareMask[board.EnPassantSquare]) != 0)
                    moveList[pos++] =  Move(fromIndex, board.EnPassantSquare, PieceType::Pawn, PieceType::Pawn);
            }

            while (targets != 0)
            {
                toIndex = Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it

                // promotions
                if ((Utils::Square::GetRankIndex(toIndex) == 7 && board.SideToMove == PieceColor::White) ||
                        (Utils::Square::GetRankIndex(toIndex) == 0 && board.SideToMove == PieceColor::Black))
                {
                    moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::Queen);
                    moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::Rook);
                    moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::Bishop);
                    moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::Knight);
                }
                else
                {
                    moveList[pos++] =  Move(fromIndex, toIndex,  board.PieceSet[toIndex].Type, PieceType::None); // no promotions
                }
            }
        }
    }

    INLINE void MoveGenerator::GetKnightMoves(BitBoard knights, Board &board, Move moveList[], int& pos, BitBoard target)
    {
        BitBoard targets;
        int fromIndex;
        int toIndex;

        while (knights != 0)
        {
            fromIndex = Utils::BitBoard::BitScanForwardReset(knights); // search for LS1B and then reset it
            targets = Knight::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex,  board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetBishopMoves(BitBoard bishops, Board &board, Move moveList[], int& pos, BitBoard target)
    {
        BitBoard targets;
        int fromIndex;
        int toIndex;

        while (bishops != 0)
        {
            fromIndex = Utils::BitBoard::BitScanForwardReset(bishops); // search for LS1B and then reset it
            targets = Bishop::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetKingMoves(BitBoard king, Board &board, Move moveList[], int &pos, BitBoard target)
    {
        BitBoard targets;
        int fromIndex;
        int toIndex;

        while (king != 0)
        {
            fromIndex = Utils::BitBoard::BitScanForwardReset(king); // search for LS1B and then reset it
            targets = King::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetRookMoves(BitBoard rooks, Board &board, Move moveList[], int &pos, BitBoard target)
    {
        BitBoard targets;
        int fromIndex;
        int toIndex;

        while (rooks != 0)
        {
            fromIndex = Utils::BitBoard::BitScanForwardReset(rooks); // search for LS1B and then reset it
            targets = Rook::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetCastleMoves(Board& board, Move moveList[], int &pos)
    {
        if (board.SideToMove == PieceColor::White)
        {
            if (board.CastlingStatus & Constants::Castle::WhiteCastleOO)
            {
                if ((Constants::Castle::WhiteCastleMaskOO & board.OccupiedSquares) == 0)
                {
                    if (!board.IsAttacked(Constants::Castle::WhiteCastleMaskOO, board.SideToMove))
                        moveList[pos++] =  Constants::Castle::WhiteCastlingOO;
                }

            }
            if (board.CastlingStatus & Constants::Castle::WhiteCastleOOO)
            {
                if ((Constants::Castle::WhiteCastleMaskOOO & board.OccupiedSquares) == 0)
                {
                    if (!board.IsAttacked(Constants::Castle::WhiteCastleMaskOOO ^ Constants::Squares::B1, board.SideToMove))
                        moveList[pos++] =  Constants::Castle::WhiteCastlingOOO;
                }
            }
        }

        else if (board.SideToMove == PieceColor::Black)
        {
            if (board.CastlingStatus & Constants::Castle::BlackCastleOO)
            {
                if ((Constants::Castle::BlackCastleMaskOO & board.OccupiedSquares) == 0)
                {
                    if (!board.IsAttacked(Constants::Castle::BlackCastleMaskOO, board.SideToMove))
                        moveList[pos++] =  Constants::Castle::BlackCastlingOO;
                }
            }
            if (board.CastlingStatus & Constants::Castle::BlackCastleOOO)
            {
                if ((Constants::Castle::BlackCastleMaskOOO & board.OccupiedSquares) == 0)
                {
                    if (!board.IsAttacked(Constants::Castle::BlackCastleMaskOOO^ Constants::Squares::B8, board.SideToMove))
                        moveList[pos++] =  Constants::Castle::BlackCastlingOOO;
                }
            }
        }
    }

    INLINE void MoveGenerator::GetLegalMoves(Move allMoves[],int& pos, Board& board)
    {
        BitBoard pinned = board.GetPinnedPieces();
        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        if (attackers)
            GetEvadeMoves(board, attackers, allMoves, pos);
        else
            GetAllMoves(allMoves, pos, board);

        int last = pos;
        int cur = 0;
        while (cur != last)
        {
            if (!board.IsMoveLegal(allMoves[cur], pinned))
            {
                allMoves[cur] = allMoves[--last];
            }
            else
            {
                cur++;
            }
        }
        pos = last;
    }

}
#endif // MOVEGENERATOR_H

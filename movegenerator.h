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
        void GetEvadeMoves(Board& board, Move moveList[], int& pos);
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
        Byte fromIndex;
        Byte toIndex;

        while (pawns != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(pawns); // search for LS1B and then reset it
            targets = Pawn::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            // en passant
            if (board.EnPassantSquare != Constants::Squares::Invalid)
            {
                epTargets = MoveDatabase::PawnAttacks[board.SideToMove][fromIndex];

                if ((epTargets & Constants::Masks::SquareMask[board.EnPassantSquare]) != 0)
                    moveList[pos++] =  Move(fromIndex, board.EnPassantSquare, PieceType::Pawn, PieceType::Pawn, PieceType::Pawn);
            }

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it

                // promotions
                if ((Utils::Square::GetRankIndex(toIndex) == 7 && board.SideToMove == PieceColor::White) ||
                        (Utils::Square::GetRankIndex(toIndex) == 0 && board.SideToMove == PieceColor::Black))
                {
                    moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Pawn, board.PieceSet[toIndex].Type, PieceType::Queen);
                    moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Pawn, board.PieceSet[toIndex].Type, PieceType::Rook);
                    moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Pawn, board.PieceSet[toIndex].Type, PieceType::Bishop);
                    moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Pawn, board.PieceSet[toIndex].Type, PieceType::Knight);
                }
                else
                {
                    moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Pawn, board.PieceSet[toIndex].Type, PieceType::None); // no promotions
                }
            }
        }
    }

    INLINE void MoveGenerator::GetKnightMoves(BitBoard knights, Board &board, Move moveList[], int& pos, BitBoard target)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (knights != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(knights); // search for LS1B and then reset it
            targets = Knight::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Knight, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetBishopMoves(BitBoard bishops, Board &board, Move moveList[], int& pos, BitBoard target)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (bishops != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(bishops); // search for LS1B and then reset it
            targets = Bishop::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Bishop, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetKingMoves(BitBoard king, Board &board, Move moveList[], int &pos, BitBoard target)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (king != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(king); // search for LS1B and then reset it
            targets = King::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, PieceType::King, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetRookMoves(BitBoard rooks, Board &board, Move moveList[], int &pos, BitBoard target)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (rooks != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(rooks); // search for LS1B and then reset it
            targets = Rook::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board) & target;

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] =  Move(fromIndex, toIndex, PieceType::Rook, board.PieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    INLINE void MoveGenerator::GetCastleMoves(Board& board, Move moveList[], int &pos)
    {
        if (board.SideToMove == PieceColor::White)
        {
            if (board.WhiteCanCastleOO)
            {
                if ((Constants::Castle::WhiteCastleMaskOO & board.OccupiedSquares) == 0)
                    moveList[pos++] =  Constants::Castle::WhiteCastlingOO;

            }
            if (board.WhiteCanCastleOOO)
            {
                if ((Constants::Castle::WhiteCastleMaskOOO & board.OccupiedSquares) == 0)
                    moveList[pos++] =  Constants::Castle::WhiteCastlingOOO;
            }
        }

        else if (board.SideToMove == PieceColor::Black)
        {
            if (board.BlackCanCastleOO)
            {
                if ((Constants::Castle::BlackCastleMaskOO & board.OccupiedSquares) == 0)
                    moveList[pos++] =  Constants::Castle::BlackCastlingOO;
            }
            if (board.BlackCanCastleOOO)
            {
                if ((Constants::Castle::BlackCastleMaskOOO & board.OccupiedSquares) == 0)
                    moveList[pos++] =  Constants::Castle::BlackCastlingOOO;
            }
        }
    }

    INLINE void MoveGenerator::GetLegalMoves(Move allMoves[],int& pos, Board& board)
    {
        BitBoard pinned = board.GetPinnedPieces();

        if (board.Attackers(board.KingSquare[board.SideToMove], board.SideToMove))
            GetEvadeMoves(board, allMoves, pos);
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

#include "movegenerator.h"
#include "move.h"
#include "board.h"
#include "defines.h"
#include "utils.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "king.h"
#include "movedatabase.h"

namespace Napoleon
{
    void MoveGenerator::GetAllMoves(Move* allMoves[], int& pos, Board& board)
    {
        GetPawnMoves(board.GetPieceSet(board.SideToMove, PieceType::Pawn), board, allMoves, pos);
        GetKnightMoves(board.GetPieceSet(board.SideToMove, PieceType::Knight), board, allMoves, pos);
        GetKingMoves(board.GetPieceSet(board.SideToMove, PieceType::King), board, allMoves, pos);
        GetBishopMoves(board.GetPieceSet(board.SideToMove, PieceType::Bishop), board, allMoves, pos);
        GetRookMoves(board.GetPieceSet(board.SideToMove, PieceType::Rook), board, allMoves, pos);
        GetQueenMoves(board.GetPieceSet(board.SideToMove, PieceType::Queen), board, allMoves, pos);
        GetCastleMoves(board, allMoves, pos);
    }

    void MoveGenerator::GetPawnMoves(BitBoard pawns, Board& board, Move* moveList[], int& pos)
    {
        BitBoard targets;
        BitBoard epTargets;
        Byte fromIndex;
        Byte toIndex;

        while (pawns != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(pawns); // search for LS1B and then reset it
            targets = Pawn::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board);

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it

                // en passant
                if (board.EnPassantSquare != Constants::Squares::Invalid)
                {
                    epTargets = board.SideToMove == PieceColor::White ? MoveDatabase::WhitePawnAttacks[fromIndex] : MoveDatabase::BlackPawnAttacks[fromIndex];

                    if ((epTargets & Constants::Masks::SquareMask[board.EnPassantSquare]) != 0)
                        moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Pawn, PieceType::Pawn, PieceType::Pawn);
                }

                // promotions
                if ((Utils::Square::GetRankIndex(toIndex) == 7 && board.SideToMove == PieceColor::White) ||
                        (Utils::Square::GetRankIndex(toIndex) == 0 && board.SideToMove == PieceColor::Black))
                {
                    moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Pawn, board.pieceSet[toIndex].Type, PieceType::Queen);
                    moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Pawn, board.pieceSet[toIndex].Type, PieceType::Rook);
                    moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Pawn, board.pieceSet[toIndex].Type, PieceType::Bishop);
                    moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Pawn, board.pieceSet[toIndex].Type, PieceType::Knight);
                }
                else
                    moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Pawn, board.pieceSet[toIndex].Type, PieceType::None); // no promotions
            }
        }
    }

    void MoveGenerator::GetKnightMoves(BitBoard knights, Board &board, Move* moveList[], int& pos)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (knights != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(knights); // search for LS1B and then reset it
            targets = Knight::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board);

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Knight, board.pieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    void MoveGenerator::GetBishopMoves(BitBoard bishops, Board &board, Move* moveList[], int& pos)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (bishops != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(bishops); // search for LS1B and then reset it
            targets = Bishop::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board);

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Bishop, board.pieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    void MoveGenerator::GetKingMoves(BitBoard king, Board &board, Move *moveList[], int &pos)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (king != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(king); // search for LS1B and then reset it
            targets = King::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board);

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] = new Move(fromIndex, toIndex, PieceType::King, board.pieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    void MoveGenerator::GetRookMoves(BitBoard rooks, Board &board, Move *moveList[], int &pos)
    {
        BitBoard targets;
        Byte fromIndex;
        Byte toIndex;

        while (rooks != 0)
        {
            fromIndex = (Byte)Utils::BitBoard::BitScanForwardReset(rooks); // search for LS1B and then reset it
            targets = Rook::GetAllTargets(Constants::Masks::SquareMask[fromIndex], board);

            while (targets != 0)
            {
                toIndex = (Byte)Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it
                moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Rook, board.pieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    void MoveGenerator::GetQueenMoves(BitBoard queens, Board &board, Move *moveList[], int &pos)
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
                moveList[pos++] = new Move(fromIndex, toIndex, PieceType::Queen, board.pieceSet[toIndex].Type, PieceType::None);
            }
        }
    }

    void MoveGenerator::GetCastleMoves(Board board, Move *moveList[], int &pos)
    {
        if (board.SideToMove == PieceColor::White)
        {
            if (board.WhiteCanCastleOO)
            {
                if ((Constants::Castle::WhiteCastleMaskOO & board.OccupiedSquares) == 0)
                    moveList[pos++] = new Move(Constants::Castle::WhiteCastlingOO);

            }
            if (board.WhiteCanCastleOOO)
            {
                if ((Constants::Castle::WhiteCastleMaskOOO & board.OccupiedSquares) == 0)
                    moveList[pos++] = new Move(Constants::Castle::WhiteCastlingOOO);
            }
        }

        else if (board.SideToMove == PieceColor::Black)
        {
            if (board.BlackCanCastleOO)
            {
                if ((Constants::Castle::BlackCastleMaskOO & board.OccupiedSquares) == 0)
                    moveList[pos++] = new Move(Constants::Castle::BlackCastlingOO);
            }
            if (board.BlackCanCastleOOO)
            {
                if ((Constants::Castle::BlackCastleMaskOOO & board.OccupiedSquares) == 0)
                    moveList[pos++] = new Move(Constants::Castle::BlackCastlingOOO);
            }
        }
    }
}

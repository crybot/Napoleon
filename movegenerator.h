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
        int MoveCount(Board&);
        template<bool>
        void GetPseudoLegalMoves(Move allMoves[],int& pos, BitBoard attackers, Board& board);
        void GetLegalMoves(Move allMoves[],int& pos, Board& board);
        void GetAllMoves(Move allMoves[], int& pos, Board& board);
        template<bool>
        void GetPawnMoves(BitBoard pawns, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetKingMoves(BitBoard king, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetKnightMoves(BitBoard knights, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetRookMoves(BitBoard rooks, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetBishopMoves(BitBoard bishops, Board& board, Move moveList[],int& pos, BitBoard target);
        void GetQueenMoves(BitBoard queens, Board& board, Move moveList[], int& pos, BitBoard target);
        void GetCastleMoves(Board& board, Move moveList[], int& pos);
        template<bool>
        void GetEvadeMoves(Board& board, BitBoard attackers, Move moveList[], int& pos);
        void GetCaptures(Move allMoves[], int& pos, Board& board);
        void GetNonCaptures(Move allMoves[], int& pos, Board& board);
    }

    inline int MoveGenerator::MoveCount(Board& board)
    {
        int count = 0;
        Move moves[Constants::MaxMoves];
        GetLegalMoves(moves, count, board);
        return count;
    }

    template<bool onlyCaptures>
    INLINE void MoveGenerator::GetPseudoLegalMoves(Move allMoves[], int& pos, BitBoard attackers, Board& board)
    {
        if (attackers)
        {
            board.IsCheck = true;
            GetEvadeMoves<onlyCaptures>(board, attackers, allMoves, pos);
        }
        else if (onlyCaptures)
        {
            GetCaptures(allMoves, pos, board);
            board.IsCheck = false;
        }
        else
        {
            GetAllMoves(allMoves, pos, board);
            board.IsCheck = false;
        }
    }

    INLINE void MoveGenerator::GetAllMoves(Move allMoves[], int& pos, Board& board)
    {
        GetCaptures(allMoves, pos, board);
        GetNonCaptures(allMoves, pos, board);
    }

    INLINE void MoveGenerator::GetCaptures(Move allMoves[], int& pos, Board& board)
    {
        BitBoard enemy = board.GetEnemyPieces();
        GetPawnMoves<true>(board.GetPieceSet(board.SideToMove, PieceType::Pawn), board, allMoves, pos, enemy);
        GetKnightMoves(board.GetPieceSet(board.SideToMove, PieceType::Knight), board, allMoves, pos, enemy);
        GetBishopMoves(board.GetPieceSet(board.SideToMove, PieceType::Bishop), board, allMoves, pos, enemy);
        GetQueenMoves(board.GetPieceSet(board.SideToMove, PieceType::Queen), board, allMoves, pos, enemy);
        GetKingMoves(board.GetPieceSet(board.SideToMove, PieceType::King), board, allMoves, pos, enemy);
        GetRookMoves(board.GetPieceSet(board.SideToMove, PieceType::Rook), board, allMoves, pos, enemy);
    }

    INLINE void MoveGenerator::GetNonCaptures(Move allMoves[], int&pos, Board& board)
    {
        BitBoard enemy = ~board.GetEnemyPieces();
        GetPawnMoves<false>(board.GetPieceSet(board.SideToMove, PieceType::Pawn), board, allMoves, pos, enemy);
        GetKnightMoves(board.GetPieceSet(board.SideToMove, PieceType::Knight), board, allMoves, pos, enemy);
        GetBishopMoves(board.GetPieceSet(board.SideToMove, PieceType::Bishop), board, allMoves, pos, enemy);
        GetQueenMoves(board.GetPieceSet(board.SideToMove, PieceType::Queen), board, allMoves, pos, enemy);
        GetKingMoves(board.GetPieceSet(board.SideToMove, PieceType::King), board, allMoves, pos, enemy);
        GetRookMoves(board.GetPieceSet(board.SideToMove, PieceType::Rook), board, allMoves, pos, enemy);
        GetCastleMoves(board, allMoves, pos);
    }

    template<bool ep>
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

            if (ep)
            {
                // en passant
                if (board.EnPassantSquare != Constants::Squares::Invalid)
                {
                    epTargets = MoveDatabase::PawnAttacks[board.SideToMove][fromIndex];

                    if ((epTargets & Constants::Masks::SquareMask[board.EnPassantSquare]) != 0)
                        moveList[pos++] =  Move(fromIndex, board.EnPassantSquare, EnPassant);
                }
            }

            while (targets != 0)
            {
                toIndex = Utils::BitBoard::BitScanForwardReset(targets); // search for LS1B and then reset it

                // promotions
                if ((Utils::Square::GetRankIndex(toIndex) == 7 && board.SideToMove == PieceColor::White) ||
                        (Utils::Square::GetRankIndex(toIndex) == 0 && board.SideToMove == PieceColor::Black))
                {
                    moveList[pos++] =  Move(fromIndex, toIndex, QueenPromotion);
                    moveList[pos++] =  Move(fromIndex, toIndex, RookPromotion);
                    moveList[pos++] =  Move(fromIndex, toIndex, BishopPromotion);
                    moveList[pos++] =  Move(fromIndex, toIndex, KnightPromotion);
                }
                else
                {
                    moveList[pos++] =  Move(fromIndex, toIndex); // no promotions
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
                moveList[pos++] = Move(fromIndex, toIndex);
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
                moveList[pos++] =  Move(fromIndex, toIndex);
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
                moveList[pos++] =  Move(fromIndex, toIndex);
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
                moveList[pos++] =  Move(fromIndex, toIndex);
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

    //thanks stockfish for this
    template<bool onlyCaptures>
    void MoveGenerator::GetEvadeMoves(Board& board, BitBoard checkers, Move moveList[], int& pos)
    {
        BitBoard b;
        int to;
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
        if (onlyCaptures)
            b = MoveDatabase::KingAttacks[ksq] & checkers;
        else
            b = MoveDatabase::KingAttacks[ksq] & ~board.GetPlayerPieces() & ~sliderAttacks;

        from = ksq;

        while (b)
        {
            to = Utils::BitBoard::BitScanForwardReset(b); // search for LS1B and then reset it
            moveList[pos++] = Move(from, to);
        }

        // Generate evasions for other pieces only if not under a double check
        if (checkersCnt > 1)
            return;

        // Blocking evasions or captures of the checking piece
        BitBoard target;

        if (onlyCaptures)
            target = checkers;
        else
            target = MoveDatabase::ObstructedTable[checksq][ksq] | checkers;

        GetPawnMoves<true>(board.GetPieceSet(board.SideToMove, PieceType::Pawn), board, moveList, pos, target);
        GetKnightMoves(board.GetPieceSet(board.SideToMove, PieceType::Knight), board, moveList, pos, target);
        GetBishopMoves(board.GetPieceSet(board.SideToMove, PieceType::Bishop), board, moveList, pos, target);
        GetRookMoves(board.GetPieceSet(board.SideToMove, PieceType::Rook), board, moveList, pos, target);
        GetQueenMoves(board.GetPieceSet(board.SideToMove, PieceType::Queen), board, moveList, pos, target);
    }


    INLINE void MoveGenerator::GetLegalMoves(Move allMoves[],int& pos, Board& board)
    {
        BitBoard pinned = board.GetPinnedPieces();
        BitBoard attackers = board.KingAttackers(board.KingSquare[board.SideToMove], board.SideToMove);

        if (attackers)
            GetEvadeMoves<false>(board, attackers, allMoves, pos);
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

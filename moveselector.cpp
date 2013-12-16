#include "moveselector.h"

namespace Napoleon
{
    MoveSelector::MoveSelector(Board& board, SearchInfo& info) : board(board), count(0), info(info), first(0)
    {

    }

    /// set scores for sorting moves
    /// 1) winning captures
    /// 2) equal captures
    /// 3) killer moves
    /// 4) other moves in history heuristic order
    /// 5) losing captures
    ///
    /// Every history move has a positive score, so to order them such that they
    /// are always after losing captures (which have a score <= 0) we found
    /// the minimum score of the captures and the maximum score of the history moves.
    /// Then we assing to each history move a score calculated with this formula:
    /// Score = HistoryScore - HistoryMax - 3
    /// The - 3 factor handle the situation where there are no losing captures,
    /// but history moves should still stay after killer moves
    /// (which have score -1 and -2). Without that, the best history move
    /// would score 0 and would be analyzed before killer moves.
    void MoveSelector::Sort(int ply)
    {
        using namespace Constants::Piece;

        int max = 0;
        int historyScore;
        Type captured;

        for (auto i=0; i<count; i++)
        {
            // MVV-LVA
            if (board.IsCapture(moves[i]))
            {
                //captured = moves[i].IsEnPassant() ? Type(PieceType::Pawn) : board.PieceOnSquare(moves[i].ToSquare()).Type;

				scores[i] = board.See(moves[i]);
            }

            else if (moves[i] == info.FirstKiller(ply))
                scores[i] = - 1;
            else if (moves[i] == info.SecondKiller(ply))
                scores[i] = - 2;

            else if ((historyScore = info.HistoryScore(moves[i], board.SideToMove())) > max)
                max = historyScore;
        }

        for (auto i=0; i<count; i++)
        {
            if (!board.IsCapture(moves[i]) && moves[i] != info.FirstKiller(ply) && moves[i] != info.SecondKiller(ply))
                scores[i] = info.HistoryScore(moves[i], board.SideToMove()) - max - 3;
        }
    }


}

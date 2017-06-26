#ifndef MOVESELECTOR_H
#define MOVESELECTOR_H
#include "constants.h"
#include "board.h"
#include "move.h"
#include "searchinfo.h"

namespace Napoleon
{
    class MoveSelector
    {
    public:
        Board& board;
        Move moves[Constants::MaxMoves];
        Move hashMove;
        int count;

        MoveSelector(Board&, SearchInfo&);

        template<bool>
        void Sort(int = 0);

        Move First();
        Move Next();
        void Reset();
        Move& operator[](int);

    private:
        int scores[Constants::MaxMoves];

        SearchInfo& info;
        int first;
    };

    inline Move& MoveSelector::operator[](int index)
    {
        return moves[index];
    }

    inline Move MoveSelector::First()
    {
        if (!hashMove.IsNull())
            return hashMove;
        else
            return Next();
    }

    inline void MoveSelector::Reset()
    {
        first = 0;
    }

    // make a selection sort on the move array for picking the best untried move
    inline Move MoveSelector::Next()
    {

        if (first == -1)
            return First();

        int max = first;

        if (max >= count)
            return Constants::NullMove;

        for (auto i=first+1; i<count; i++)
            if (scores[i] > scores[max])
                max = i;

        if (max != first)
        {
            std::swap(moves[first], moves[max]);
            std::swap(scores[first], scores[max]);
        }

        Move move = moves[first++];

        if (move != hashMove)
            return move;
        else
            return Next();
    }

    /// set scores for sorting moves
    /// 1) promotions
    /// 2) winning captures
    /// 3) equal captures
    /// 4) killer moves
    /// 5) other moves in history heuristic order
    /// 6) losing captures
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

    template<bool quiesce>
    void MoveSelector::Sort(int ply)
    {
        using namespace Constants::Piece;

        int max = 0;
        int historyScore;

        for (auto i=0; i<count; i++)
        {
            if (moves[i].IsPromotion())
            {
                scores[i] =  Constants::Piece::PieceValue[moves[i].PiecePromoted()];
            }

            else if (board.IsCapture(moves[i]))
            {
                if (quiesce)
                {
                    Type captured = moves[i].IsEnPassant() ? Type(PieceType::Pawn) : board.PieceOnSquare(moves[i].ToSquare()).Type; // MVV-LVA
                    scores[i] = PieceValue[captured] - PieceValue[board.PieceOnSquare(moves[i].FromSquare()).Type];
                }
                else
                {
                    scores[i] = board.See(moves[i]); // SEE
                }
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

#endif // MOVESELECTOR_H

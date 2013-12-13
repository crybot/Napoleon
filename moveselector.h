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
        Move nextMove;
        int count;

        MoveSelector(Board&, SearchInfo&);

        void Sort(int = 0);

        Move Next();
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

    // make a selection sort on the move array for picking the best untried move
    inline Move MoveSelector::Next()
    {
        int max = first;

        for (auto i=first+1; i<count; i++)
            if (scores[i] > scores[max])
                max = i;

        if (max != first)
        {
            std::swap(moves[first], moves[max]);
            std::swap(scores[first], scores[max]);
        }

        return moves[first++];
    }
}

#endif // MOVESELECTOR_H

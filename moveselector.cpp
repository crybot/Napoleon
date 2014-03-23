#include "moveselector.h"

namespace Napoleon
{

    MoveSelector::MoveSelector(Board& board, SearchInfo& info) : board(board), hashMove(Constants::NullMove), count(0), info(info), first(0)
    {

    }

}

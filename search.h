#ifndef SEARCH_H
#define SEARCH_H

namespace Napoleon
{
    class Board;
    class Search
    {
    public:
        static int negaMax(int depth, int alpha, int beta, Board& board);
        static int quiesce(int alpha, int benta, Board& board);
    };

}

#endif // SEARCH_H

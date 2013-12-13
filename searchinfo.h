#ifndef SEARCHINFO_H
#define SEARCHINFO_H
#include "constants.h"
#include "stopwatch.h"
#include "piece.h"

namespace Napoleon
{

    class Move;
    class SearchInfo
    {
    public:
        enum class Time : int { Infinite = -1 };

        SearchInfo(int time = int(Time::Infinite), int nodes = 0, int maxDepth = 1);

        void NewSearch(int time = int(Time::Infinite));
        int IncrementDepth();
        int MaxDepth();
        int Nodes();
        bool TimeOver();

        void ResetNodes();
        void VisitNode();
        void SetKillers(Move, int);
        void SetHistory(Move, Color, int);

        Move FirstKiller(int);
        Move SecondKiller(int);

        int HistoryScore(Move, Color);

        double ElapsedTime();

    private:
        int nodes;
        int maxDepth;
        int maxPly;
        int history[2][64*64];
        int allocatedTime; // milliseconds
        Move killers[Constants::MaxPly][2];
        StopWatch timer;
    };

    inline bool SearchInfo::TimeOver()
    {
        if (allocatedTime == int(Time::Infinite))
            return false;

        return (timer.ElapsedMilliseconds() >= allocatedTime || timer.ElapsedMilliseconds() / allocatedTime >= 0.85);
    }

    inline int SearchInfo::Nodes()
    {
        return nodes;
    }

    inline void SearchInfo::VisitNode()
    {
        ++nodes;
    }

    inline Move SearchInfo::FirstKiller(int depth)
    {
        return killers[depth][0];
    }

    inline Move SearchInfo::SecondKiller(int depth)
    {
        return killers[depth][1];
    }

    inline int SearchInfo::HistoryScore(Move move, Color color)
    {
        return history[color][move.ButterflyIndex()];
    }

    inline double SearchInfo::ElapsedTime()
    {
        return timer.ElapsedMilliseconds();
    }

    inline void SearchInfo::SetKillers(Move move, int depth)
    {
        if (move != killers[depth][0])
        {
            killers[depth][1] = killers[depth][0];
        }
        killers[depth][0] = move;
    }

    inline void SearchInfo::SetHistory(Move move, Color color, int depth)
    {
        history[color][move.ButterflyIndex()] += (1 << depth);
    }
}
#endif // SEARCHINFO_H

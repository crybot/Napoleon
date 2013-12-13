#include "searchinfo.h"
#include <cstring>

namespace Napoleon
{
    SearchInfo::SearchInfo(int time, int nodes, int maxDepth) : nodes(nodes), maxDepth(maxDepth)
    {
        allocatedTime = time;
    }

    int SearchInfo::IncrementDepth()
    {
        return maxDepth++;
    }

    int SearchInfo::MaxDepth()
    {
        return maxDepth;
    }

    void SearchInfo::NewSearch(int time)
    {
        allocatedTime = time;

        nodes = 0;
        maxDepth = 1;

        std::memset(history, 0, sizeof(history));
        std::memset(killers, 0, sizeof(killers));

        timer.Restart();
    }

    void SearchInfo::ResetNodes()
    {
        nodes = 0;
    }


}

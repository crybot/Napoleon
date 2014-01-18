#include "searchinfo.h"
#include <cstring>
#include <iostream>

namespace Napoleon
{
    SearchInfo::SearchInfo(int time, int maxDepth, int nodes) :maxDepth(maxDepth), nodes(nodes)
    {
        allocatedTime = time;
        SetDepthLimit(100);
    }

    int SearchInfo::IncrementDepth()
    {
        return maxDepth++;
    }

    int SearchInfo::MaxDepth()
    {
        return maxDepth;
    }

    void SearchInfo::SetDepthLimit(int depth)
    {
        depthLimit = depth;
    }

    void SearchInfo::NewSearch(int time)
    {
        ResetNodes();
        allocatedTime = time;

        maxDepth = 1;

        std::memset(history, 0, sizeof(history));
        std::memset(killers, 0, sizeof(killers));

        timer.Restart();
    }

    void SearchInfo::StopSearch()
    {
        SetDepthLimit(100);
    }

    void SearchInfo::ResetNodes()
    {
        nodes = 0;
    }


}

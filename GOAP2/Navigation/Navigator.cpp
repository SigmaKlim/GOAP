#include "Navigator.h"
#include "../Attributes/Special/AAtNode.h"



void Navigator::AddNodes(const std::string& name, const std::unordered_set<int>& ids)
{
    auto fres = nodeMap.find(name);
    if (fres == nodeMap.end())
        nodeMap.insert({name, ids});
    else
        for (auto& id : ids)
            assert(fres->second.insert(id).second);
}

const std::unordered_set<int>& Navigator::GetNodesByName(const std::string& name) const
{
    auto fres = nodeMap.find(name);
    assert(fres != nodeMap.end());
    return fres->second;
}

void Navigator::SetDistance(int from, int to, float distance)
{
    distances.insert_or_assign(std::make_pair(from, to), distance);
    if (IsSymmetric == true)
        distances.insert_or_assign(std::make_pair(to, from), distance);
    if (distance > maxDistance)
        maxDistance = distance;
}

float Navigator::GetDistance(int from, int to) const
{
    auto fres = distances.find(std::make_pair(from, to));
    assert(fres != distances.end());
    return fres->second;
}

float Navigator::GetMaxDistance() const
{
    return maxDistance;
}
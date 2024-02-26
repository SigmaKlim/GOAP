#include "Planner.h"

#include "Condition/ConditionSet.h"

void Planner::GetNeighbors(std::vector<Vertex>& neighbors, const Vertex& vertex, const Vertex& finish) const
{
    for (unsigned i = 0; i < actionCatalogue.Size(); i++)
    {
        ConditionSet newConditionSet(vertex.ActiveConditionSet.Size());
        bool isActionUseful = vertex.ActiveConditionSet.Reduce((*actionCatalogue.GetItem(i))->GetEffects(), newConditionSet);
        if (isActionUseful)
            neighbors.push_back({newConditionSet, i, *actionCatalogue.GetName(i)});
    }
}

bool Planner::Satisfies(const Vertex& vertex/*active cond set*/, const Vertex& finish/*start state*/) const
{
    ConditionSet dummy(vertex.ActiveConditionSet);
    return vertex.ActiveConditionSet.Reduce(dynamic_cast<const WorldMask&>(finish.ActiveConditionSet), dummy); 
}

float Planner::GetDistance(const Vertex& from, const Vertex& to) const
{
    return (*actionCatalogue.GetItem(to.PrevActionId))->GetCost();
}

float Planner::GetDistanceDenominator() const
{
    static float accumActionCostSum = 0.0f;
    if (accumActionCostSum == 0.0f)
        for (auto& action : actionCatalogue.ObjectIterator)
            accumActionCostSum += action->GetMaxCost();
    return accumActionCostSum;
}

float Planner::GetHeuristic(const Vertex& vertex, const Vertex& finish) const
{
    return 0.0f;
}

float Planner::GetHeuristicsDenominator() const
{
    return AStarSolver<Vertex>::GetHeuristicsDenominator();
}


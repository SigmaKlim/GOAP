#include "Planner.h"

#include "Condition/ConditionSet.h"
#include "Condition/ConditionImpl.h"

Catalogue<IAttribute*>* ValueSet::AttributeCataloguePtr = nullptr;


Plan::Plan(const Planner& planner, const ValueSet& startState, const std::string& goalName) :
                                StartState(startState), Goal(*planner._goalCatalogue.GetItem(goalName))
{
    GoalName = goalName;
    Cost = 0.0f;
}

std::size_t hash_value(const Vertex& vertex)
{
    size_t hash = 0;
    boost::hash_combine(hash, vertex.ActiveConditionSet);
    boost::hash_combine(hash, vertex.PrevActionId);
    boost::hash_combine(hash, vertex.UserData);
    return hash;
}

Planner::Planner()
{
    ValueSet::AttributeCataloguePtr = &_attributeCatalogue;
}

Plan Planner::ConstructPlan(const ValueSet& startState, std::string goalName, const ActionData& initData) const
{
    Plan plan(*this, startState, goalName);
    if (startState.NumAffected() != startState.Size())
        plan.success = false;
    else
    {
        //We convert startState to a condition set made of Equal conditions
        ConditionSet targetConditionSet(_attributeCatalogue.Size());
        for (unsigned i = 0; i < _attributeCatalogue.Size(); i++)
            targetConditionSet.SetCondition(i, new Equal(plan.StartState.GetProperty(i)));
        Vertex targetVertex(targetConditionSet);
        Vertex departureVertex( plan.Goal,
                                std::numeric_limits<size_t>::max(),
                                initData,
                                "");
        Path<Vertex> path;
        plan.success = Pathfind(path, departureVertex, targetVertex, &plan.TelemetryData);
        if (plan.success != false)
            for (size_t i = path.Vertices.size() - 1; i > 0; i--) //iterate from back to start, because we build plan from the last action to the first
            {
                plan.ActionIds.push_back(path.Vertices[i].PrevActionId);
                plan.ActionNames.push_back(*_actionConstructorCatalogue.GetName(path.Vertices[i].PrevActionId));
                plan.Cost = path.Cost;
            }
    }
    return plan;
}

void Planner::GetNeighbors(std::vector<Vertex>& neighbors, std::vector<float>& distances, const Vertex& vertex, const Vertex& finish) const
{
    for (unsigned i = 0; i < _actionConstructorCatalogue.Size(); i++)
    {
        std::vector<Action> actions;
        (*_actionConstructorCatalogue.GetItem(i))->ConstructActions(actions, vertex.ActiveConditionSet, vertex.UserData);
        for (auto& action : actions)
        {
            ConditionSet reducedConditionSet(vertex.ActiveConditionSet.Size());
            bool isActionUseful = vertex.ActiveConditionSet.Reduce(action.Effects, reducedConditionSet);
            ConditionSet mergedConditionSet(vertex.ActiveConditionSet.Size());
            bool isActionLegit = reducedConditionSet.Merge(action.Conditions, mergedConditionSet);
            if (isActionUseful == true && isActionLegit == true)
            {
                neighbors.push_back(    {mergedConditionSet, i, action.UserData,
                                        *_actionConstructorCatalogue.GetName(i)});
                distances.push_back(action.Cost);
            }
        }
    }
}

bool Planner::Satisfies(const Vertex& vertex/*active cond set*/, const Vertex& finish/*start state*/) const
{
    return !vertex.ActiveConditionSet.HasConflicts(finish.ActiveConditionSet);
}

float Planner::GetDistanceDenominator() const
{
    static float accumActionCostSum = 0.0f;
    if (accumActionCostSum == 0.0f)
        for (auto& action : _actionConstructorCatalogue.ObjectIterator)
            accumActionCostSum += action->GetMaxCost();
    return accumActionCostSum;
}

float Planner::GetHeuristic(const Vertex& vertex/*active cond set*/, const Vertex& finish/*start state*/) const
{
    return 0.0f;
}

float Planner::GetHeuristicsDenominator() const
{
    return AStarSolver<Vertex>::GetHeuristicsDenominator();
}

size_t Planner::GetNumAttributes() const
{
    return _attributeCatalogue.Size();
}

size_t Planner::GetAttributeId(const std::string& name) const
{
    return *_attributeCatalogue.GetId(name);
}




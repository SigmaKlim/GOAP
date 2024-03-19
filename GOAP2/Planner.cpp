#include "Planner.h"

#include "Conditions/Basic/ConditionSet.h"
#include "Conditions/Special/CEqual.h"


Plan::Plan(size_t numAttributes) :
    StartState(numAttributes), Goal(numAttributes),
    ResultState(numAttributes)
{
}


template<>
size_t VertexKey<Vertex>::operator()(const Vertex& k) const
{
    size_t hash = 0;
    for (size_t i = 0; i < k.ActiveConditionSet.Size(); i++)
        if (k.ActiveConditionSet.IsAffected(i))
            MathHelper::CombineHash(hash, k.ActiveConditionSet.GetProperty(i));
    MathHelper::CombineHash(hash, k.PrevActionId);
    return hash;
}

Planner::Planner(const std::vector<std::shared_ptr<IAttribute>>& attributes,
    const std::vector<std::shared_ptr<IAction>>& actions,
    const std::vector<const std::string*>& actionNames) : _attributes(attributes), _actions(actions), _actionNames(actionNames)
{
    ConditionSet::_attributes= &this->_attributes;
}


bool Planner::ConstructPlan(Plan& plan, SupplementalData initData) const
{
    assert(plan.StartState.NumAffected() == plan.StartState.Size());
    //We convert startState to a condition set made of Equal conditions
    ConditionSet targetConditionSet(_attributes.size());
    for (unsigned i = 0; i < _attributes.size(); i++)
        targetConditionSet.SetCondition(i, new CEqual(plan.StartState.GetProperty(i)));
    ActionInstanceData dummy(ConditionSet(_attributes.size()), ValueSet(_attributes.size()), 0.0f, SupplementalData(), "");
    Vertex targetVertex(targetConditionSet,
                            std::numeric_limits<size_t>::max(),
                            dummy,
                            0,
                            "");
    ActionInstanceData initDummy(ConditionSet(_attributes.size()), ValueSet(_attributes.size()), 0.0f, initData, "");
    Vertex departureVertex( plan.Goal,
                            std::numeric_limits<size_t>::max(),
                            initDummy,
                            0,
                            "");
    Path<Vertex> path;
    plan.Success = Pathfind(path, departureVertex, targetVertex, &plan.TelemetryData);
    if (plan.Success == true)
    {
        ValueSet prevState = plan.StartState;
        for (size_t i = path.Vertices.size() - 1; i > 0; i--) //iterate from back to start, because we build plan from the last action to the first
        {
            plan.ActionIds.push_back(path.Vertices[i].PrevActionId);
            plan.ActionNames.push_back(*_actionNames[path.Vertices[i].PrevActionId]);
            auto anotherActionInstance = _actions[path.Vertices[i].PrevActionId]->ConstructActionInstancePosteriori(prevState, path.Vertices[i].PrevActionInstance);
            plan.ActionInstances.push_back(anotherActionInstance);
            prevState.Modify(anotherActionInstance.Effects);
        }
        plan.TotalCost = path.Cost;
        plan.ResultState = prevState;
        return true;
    }
    return false;
}

void Planner::GetNeighbors(std::vector<Vertex>& neighbors, std::vector<float>& distances, const Vertex& vertex, const Vertex& finish) const
{
    if (vertex.ActionCtr > MAX_NUM_ACTIONS_PER_PLAN)
        return;
    for (unsigned i = 0; i < _actions.size(); i++)
    {
        std::vector<ActionInstanceData> actions;
        _actions[i]->ConstructActionInstancesPriori(actions, vertex.ActiveConditionSet, vertex.PrevActionInstance.UserData);
        for (auto& action : actions)
        {
            ConditionSet reducedConditionSet(vertex.ActiveConditionSet.Size());
            bool isActionUseful = vertex.ActiveConditionSet.Reduce(action.Effects, reducedConditionSet, action.UserData);
            ConditionSet mergedConditionSet(vertex.ActiveConditionSet.Size());
            bool isActionLegit = reducedConditionSet.Merge(action.Conditions, mergedConditionSet);
            if (isActionUseful == true && isActionLegit == true)
            {
                neighbors.push_back({   mergedConditionSet, i,
                                        action,
                                        vertex.ActionCtr + 1,
                                        *_actionNames[i]});
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
        for (auto& action : _actions)
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



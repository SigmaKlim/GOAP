#include "GPlanner.h"
#include <iostream>

#include "NavPathfinder.h"


const std::vector<std::string>& Plan::GetActionSequence() const
{
    return _actionNames;
}

unsigned Plan::GetCost() const
{
    return _cost;
}


GPlanner::GPlanner()
{
    WorldState::_planner = this;
}

bool GPlanner::RegisterAttribute(const std::string& name, const Attribute& attribute)
{
    bool contains =  (_attributeCatalogue.find(name) != _attributeCatalogue.end());
    if (contains)
    {
        std::cout << "The Planner already contains attribute \"" + name + "\".\n";
        return false;
    }
    bool isOutOfRange = (_attributeCatalogue.size() == MAX_ATTRIBUTES);
    if (isOutOfRange == true)
    {
        std::cout << "Can't register attribute \"" + name + 
            "\": number of attributes reached MAX_ATTRIBUTES(" + std::to_string(MAX_ATTRIBUTES) + ").\n";
        return false;
    }
    _attributeCatalogue.insert(std::make_pair(std::string(name), attribute));
    WorldState::_attributeNames.emplace(std::string(name));
    WorldState::_numAttributes++;
    return true;
}

bool GPlanner::RegisterAttribute(const std::string& name, const std::vector<std::string>& enumerators)
{
    bool contains = (_attributeCatalogue.find(name) != _attributeCatalogue.end());
    if (contains)
    {
        std::cout << "The Planner already contains attribute \"" + name + "\".\n";
        return false;
    }
    bool isOutOfRange = _attributeCatalogue.size() == MAX_ATTRIBUTES;
    if (isOutOfRange == true)
    {
        std::cout << "Can't register attribute \"" + name +
            "\": number of attributes reached MAX_ATTRIBUTES(" + std::to_string(MAX_ATTRIBUTES) + ").\n";
        return false;
    }
    _attributeCatalogue.emplace(std::make_pair(std::string(name), Attribute(enumerators)));
    WorldState::_attributeNames.emplace(std::string(name));
    WorldState::_numAttributes++;
    return true;
}

bool GPlanner::RegisterAction(const std::string& name, const Action& action)
{
    bool contains =  (_actionCatalogue.find(name) != _actionCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains action \"" + name + "\".\n";
        return false;
    }
    _actionCatalogue.insert(std::make_pair(std::string(name), action ));
    return true;
}

bool GPlanner::RegisterAction(const std::string& name, const WorldState& cnd, const WorldState& eff, unsigned cost)
{
    bool contains =  (_actionCatalogue.find(name) != _actionCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains action \"" + name + "\".\n";
        return false;
    }
    _actionCatalogue.insert(std::make_pair(std::string(name), Action(cnd, eff, cost)));
    return true;
}

bool GPlanner::RegisterAction(const std::string& name, const WorldState& cnd,
    const std::set<std::string>& affectedAttributes, WorldState(* EvaluateEffect)(ActionInputDataBase* data),
    unsigned(* CalculateCost)(ActionInputDataBase* data))
{
    bool contains =  (_actionCatalogue.find(name) != _actionCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains action \"" + name + "\".\n";
        return false;
    }
    BitMask affectedAttributesMask = BitMask::MakeAllZeros(WorldState::_numAttributes);
    for (auto& attributeName : affectedAttributes)
    {
        BitMask thisAttributeMask = BitMask::MakeRightOnes(WorldState::_numAttributes, Attribute::MAX_VALUES);
        unsigned attributeIndex = WorldState::FindAttribute(attributeName);
        thisAttributeMask <<= attributeIndex;
        affectedAttributesMask |= thisAttributeMask;
    }
    _actionCatalogue.insert(std::make_pair(std::string(name), Action(cnd, affectedAttributesMask, EvaluateEffect, CalculateCost)));
    return true;
}

bool GPlanner::RegisterGoal	(const std::string& name, const WorldState& goal_)
{
    bool contains = (_goalCatalogue.find(name) != _goalCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains goal \"" + name + "\".\n";
        return false;
    }
    _goalCatalogue.insert(std::make_pair(std::string(name), goal_ ));
    return true;
}

bool GPlanner::RegisterGoal(const std::string& name, const std::unordered_map<std::string, std::string>& nameValuePairs)
{
    bool contains = (_goalCatalogue.find(name) != _goalCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains goal \"" + name + "\".\n";
        return false;
    }
    _goalCatalogue.insert(std::make_pair(std::string(name), WorldState(nameValuePairs)));
    return true;
}

const Attribute& GPlanner::GetAttribute(const std::string& name) const
{
    auto search = _attributeCatalogue.find(name);
    if (search == _attributeCatalogue.end())
    {
        std::cout << "Attribute \"" + name + "\" is not in the catalogue\n";
        return Attribute();
    }
    return search->second;
}

const Action& GPlanner::GetAction(const std::string& name) const
{
    auto search = _actionCatalogue.find(name);
    if (search == _actionCatalogue.end())
    {
        std::cout << "Action \"" + name + "\" is not in the catalogue\n";
        return Action();
    }
    return search->second;
}

const WorldState& GPlanner::GetGoal(const std::string& name) const
{
    auto search = _goalCatalogue.find(name);
    if (search == _goalCatalogue.end())
    {
        std::cout << "Goal \"" + name + "\" is not in the catalogue\n";
        return WorldState();
    }
    return search->second;
}

bool GPlanner::ConstructPlan(Plan& plan, TelemetryData* telemetryData, void* userData) const
{
    Path<Vertex> path;
    const auto& goal = GetGoal(plan.GoalName);
    std::set<std::string> availableActionNames;
    for (auto& actionName : _actionCatalogue)
        availableActionNames.insert(actionName.first);
    Vertex start(goal, availableActionNames, ""); // we build the path from goal state to current state
    Vertex finish(plan.StartingWs, {}, "");
    bool foundPath = Pathfind(path, start, finish, telemetryData, userData);
    if (foundPath == false)
        return false;
    auto planLength = path.Vertices.size() - 1;
    plan._actionNames.resize(planLength);
    for (auto i = 1; i < path.Vertices.size(); i++)
        plan._actionNames[i - 1] = path.Vertices[path.Vertices.size() - i].prevAction;
    plan._cost = path.Cost;
    return true;
}

void GPlanner::GetNeighbors(std::vector<Vertex>& neighbors, const Vertex& vertex, const Vertex& finish, void* userData) const
{
    ActionInputDataBase* actionInputData = static_cast<ActionInputDataBase*>(userData);
    for (auto& actionName : vertex.availableActionNames)
    {
        WorldState nextState; //change state by action
        auto& action = GetAction(actionName);
        std::string debugName = actionName;
        auto debugSize = std::string("SearchEnemy").size();
        if (actionName == "AttackWeapon")
            std::cout << "";
        actionInputData->DesiredStateMask = &(vertex.activeConditionSet.GetValueMask());
        actionInputData->AffectedAttributesMask = &action._effect._affectedAttributesMask;
        if (IsActionUseful(nextState, vertex.activeConditionSet, action, actionInputData)) //check if nextState is closer to finish_ than vertex_.state and does not corrupt conditionSet
        {
            auto neighborAvailableActions = vertex.availableActionNames;
            neighborAvailableActions.erase(actionName);
            neighbors.emplace_back(nextState, neighborAvailableActions, actionName);
        }
    }
}

bool GPlanner::Satisfies(const Vertex& vertex, const Vertex& targetVertex, void* userData) const
{
    const auto& initialState = targetVertex.activeConditionSet;
    const auto& activeConditionSet = vertex.activeConditionSet;
    BitMask significantInitialStateMask = initialState._valueMask & activeConditionSet._affectedAttributesMask;
    return (significantInitialStateMask & activeConditionSet._valueMask) == significantInitialStateMask;
}

BitMask GPlanner::GetId(const Vertex& vertex) const
{
    return vertex.activeConditionSet._valueMask;
}

unsigned GPlanner::GetDistance(const Vertex& from, const Vertex& to, void* userData) const
{
    auto* actionData = static_cast<ActionInputDataBase*>(userData);
    actionData->DestinationPointName = from.activeConditionSet
    auto& action = GetAction(to.prevAction);
    return action.GetOrCalculateCost(actionData);
}

unsigned GPlanner::GetHeuristic(const Vertex& vertex, const Vertex& target, void* userData) const
{
    unsigned diff = 0;
    for (unsigned i = 0; i < WorldState::_numAttributes; i++)
    {
        unsigned conditionAttribute = vertex.activeConditionSet.GetAttributeMask(i);
        unsigned targetAttribute = target.activeConditionSet.GetAttributeMask(i);
        diff += (conditionAttribute != (targetAttribute & conditionAttribute));
    }
    return diff;
    //return 0;
}

bool GPlanner::IsActionUseful(WorldState& modifiedConditionSet, const WorldState& conditionSet, const Action& action, ActionInputDataBase* actionData) const
{
    BitMask significantConditionSet = conditionSet._valueMask & action.GetOrEvaluateEffect(actionData)._affectedAttributesMask; //leave only conditions affected by effects of the action
    BitMask significantActionEffects = conditionSet._affectedAttributesMask & action.GetOrEvaluateEffect(actionData)._valueMask; //leave only effects that influence conditions from the condition set
    if ((significantActionEffects & significantConditionSet) != significantActionEffects 
        || significantConditionSet == BitMask::MakeAllZeros(significantConditionSet.GetNumBits())) // check if the action effects don't violate conditions from the set and if there are any affected conditions at all 
            return false;
    //modifiedConditionSet._valueMask = conditionSet._valueMask & ~action._effect._valueMask;
    modifiedConditionSet._valueMask = conditionSet._valueMask & ~action.GetOrEvaluateEffect(actionData)._affectedAttributesMask; //remove fulfilled conditions from the set
    modifiedConditionSet._affectedAttributesMask = conditionSet._affectedAttributesMask & ~action.GetOrEvaluateEffect(actionData)._affectedAttributesMask; //remove fulfilled conditions from the set
    BitMask significantActionConditions = modifiedConditionSet._affectedAttributesMask & action.GetOrEvaluateEffect(actionData)._valueMask; //leave only conditions on attributes shared between action conditions and conditions set
    significantConditionSet = conditionSet._valueMask & action.GetOrEvaluateEffect(actionData)._affectedAttributesMask;
    if ((significantActionConditions & significantConditionSet) != significantActionConditions) // check if the action conditions don't violate conditions from the set 
        return false;
    modifiedConditionSet._valueMask |= action.GetOrEvaluateEffect(actionData)._valueMask;
    modifiedConditionSet._affectedAttributesMask |= action.GetOrEvaluateEffect(actionData)._affectedAttributesMask;
    return true;
}



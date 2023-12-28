#include "GPlanner.h"
#include <iostream>
#include "Actions/SimpleAction.h"
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

bool GPlanner::RegisterAction(const std::string& name, IAction& action)
{
    bool contains =  (_actionCatalogue.find(name) != _actionCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains action \"" + name + "\".\n";
        return false;
    }
    _actionCatalogue.insert({std::string(name), action});
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

const IAction& GPlanner::GetAction(const std::string& name) const
{
    auto search = _actionCatalogue.find(name);
    if (search == _actionCatalogue.end())
    {
        std::cout << "Action \"" + name + "\" is not in the catalogue\n";
        return SimpleAction();
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
    for (unsigned i = 1; i < path.Vertices.size(); i++)
    {
        unsigned index = path.Vertices.size() - i;
        plan._actionNames[i - 1] = path.Vertices[index].PrevActionName;
        if (index > 0)
            plan._actionNames[i - 1] += " " + GetAction(plan._actionNames[i - 1]).GetPostfixName(path.Vertices[index - 1].ActiveConditionSet);
        else
            plan._actionNames[i - 1] += " " + GetAction(plan._actionNames[i - 1]).GetPostfixName(plan.StartingWs);
    }
    plan._cost = path.Cost;
    return true;
}

const std::unordered_map<std::string, Attribute>& GPlanner::GetAttributeCatalogue() const
{
    return _attributeCatalogue;
}

void GPlanner::GetNeighbors(std::vector<Vertex>& neighbors, const Vertex& vertex, const Vertex& finish) const
{
    for (auto& actionName : vertex.AvailableActionNames)
    {
        WorldState nextState; //change state by action
        auto& action = GetAction(actionName);
        if (actionName == "TakeCover")
            std::cout << "";
        if (IsActionUseful(nextState, vertex.ActiveConditionSet, action)) //check if nextState is closer to finish_ than vertex_.state and does not corrupt conditionSet
        {
            auto neighborAvailableActions = vertex.AvailableActionNames;
            //neighborAvailableActions.erase(actionName);
            neighbors.emplace_back(nextState, neighborAvailableActions, actionName);
        }
    }
}

bool GPlanner::Satisfies(const Vertex& vertex, const Vertex& targetVertex) const
{
    const auto& initialState = targetVertex.ActiveConditionSet;
    const auto& activeConditionSet = vertex.ActiveConditionSet;
    BitMask significantInitialStateMask = initialState._valueMask & activeConditionSet._affectedAttributesMask;
    return (significantInitialStateMask & activeConditionSet._valueMask) == significantInitialStateMask;
}

BitMask GPlanner::GetId(const Vertex& vertex) const
{
    return vertex.ActiveConditionSet._valueMask;
}

unsigned GPlanner::GetDistance(const Vertex& from, const Vertex& to) const
{
    auto& action = GetAction(to.PrevActionName);
    CalculateActionCostInputData actionData;
    actionData.prevState = &from.ActiveConditionSet;
    return action.GetCost(&actionData);
}

unsigned GPlanner::GetHeuristic(const Vertex& vertex, const Vertex& target, void* userData) const
{
    unsigned diff = 0;
    for (unsigned i = 0; i < WorldState::_numAttributes; i++)
    {
        unsigned conditionAttribute = vertex.ActiveConditionSet.GetAttributeMask(i);
        unsigned targetAttribute = target.ActiveConditionSet.GetAttributeMask(i);
        diff += (conditionAttribute != (targetAttribute & conditionAttribute));
    }
    return diff;
    //return 0;
}

bool GPlanner::IsActionUseful(WorldState& modifiedConditionSet, const WorldState& conditionSet, const IAction& action) const
{
    EvaluateActionEffectInputData actionData;
    actionData.DesiredStateMask = &conditionSet;
    BitMask significantConditionSet = conditionSet._valueMask & action.GetEffect(&actionData)._affectedAttributesMask; //leave only conditions affected by effects of the action
    BitMask significantActionEffects = conditionSet._affectedAttributesMask & action.GetEffect(&actionData)._valueMask; //leave only effects that influence conditions from the condition set
    if ((significantActionEffects & significantConditionSet) != significantActionEffects 
        || significantConditionSet == BitMask::MakeAllZeros(significantConditionSet.GetNumBits())) // check if the action effects don't violate conditions from the set and if there are any affected conditions at all 
            return false;
    //modifiedConditionSet._valueMask = conditionSet._valueMask & ~action._effect._valueMask;
    modifiedConditionSet._valueMask = conditionSet._valueMask & ~action.GetEffect(&actionData)._affectedAttributesMask; //remove fulfilled conditions from the set
    modifiedConditionSet._affectedAttributesMask = conditionSet._affectedAttributesMask & ~action.GetEffect(&actionData)._affectedAttributesMask; //remove fulfilled conditions from the set
    BitMask significantActionConditions = modifiedConditionSet._affectedAttributesMask & action.GetCondition()._valueMask; //leave only conditions on attributes shared between action conditions and conditions set
    significantConditionSet = conditionSet._valueMask & action.GetCondition()._affectedAttributesMask;
    if ((significantActionConditions & significantConditionSet) != significantActionConditions) // check if the action conditions don't violate conditions from the set 
        return false;
    modifiedConditionSet._valueMask |= action.GetCondition()._valueMask;
    modifiedConditionSet._affectedAttributesMask |= action.GetCondition()._affectedAttributesMask;
    return true;
}



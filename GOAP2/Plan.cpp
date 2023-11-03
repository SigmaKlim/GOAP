#include "Plan.h"
#include <iostream>


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

bool GPlanner::ConstructPlan(Plan& plan_, TelemetryData* telemetryData) const
{
    Path<Vertex> path;
    const auto& goal = GetGoal(plan_.GoalName);
    std::set<std::string> availableActionNames;
    for (auto& actionName : _actionCatalogue)
        availableActionNames.insert(actionName.first);
    Vertex start(goal, availableActionNames, ""); // we build the path from goal state to current state
    Vertex finish(plan_.StartingWs, {}, "");
    bool foundPath = Pathfind(path, start, finish, telemetryData);
    if (foundPath == false)
        return false;
    auto planLength = path.Vertices.size() - 1;
    plan_._actionNames.resize(planLength);
    for (auto i = 1; i < path.Vertices.size(); i++)
        plan_._actionNames[i - 1] = path.Vertices[path.Vertices.size() - i].prevAction;
    plan_._cost = path.Cost;
    return true;
}



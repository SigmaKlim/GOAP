#include "Plan.h"
#include <iostream>
#include <stdbool.h>


const std::vector<std::string>& Plan::GetActionSequence() const
{
    return actionNames;
}

unsigned Plan::GetCost() const
{
    return cost;
}

Planner::Planner()
{
    WorldState::planner = this;
}

Planner::~Planner()
{
}

bool Planner::RegisterAttribute(const std::string& name_, const Attribute& attribute_)
{
    bool contains =  (attributeCatalogue.find(name_) != attributeCatalogue.end());
    if (contains)
    {
        std::cout << "The Planner already contains attribute \"" + name_ + "\".\n";
        return false;
    }
    bool isOutOfRange = (attributeCatalogue.size() == MAX_ATTRIBUTES);
    if (isOutOfRange == true)
    {
        std::cout << "Can't register attribute \"" + name_ + 
            "\": number of attributes reached MAX_ATTRIBUTES(" + std::to_string(MAX_ATTRIBUTES) + ").\n";
        return false;
    }
    attributeCatalogue.insert(std::make_pair(std::string(name_), attribute_));
    WorldState::attributeNames.emplace(std::string(name_));
    WorldState::numAttributes++;
    return true;
}

bool Planner::RegisterAttribute(const std::string& name_, const std::vector<std::string>& enumerators_)
{
    bool contains = (attributeCatalogue.find(name_) != attributeCatalogue.end());
    if (contains)
    {
        std::cout << "The Planner already contains attribute \"" + name_ + "\".\n";
        return false;
    }
    bool isOutOfRange = attributeCatalogue.size() == MAX_ATTRIBUTES;
    if (isOutOfRange == true)
    {
        std::cout << "Can't register attribute \"" + name_ +
            "\": number of attributes reached MAX_ATTRIBUTES(" + std::to_string(MAX_ATTRIBUTES) + ").\n";
        return false;
    }
    attributeCatalogue.emplace(std::make_pair(std::string(name_), Attribute(enumerators_)));
    WorldState::attributeNames.emplace(std::string(name_));
    WorldState::numAttributes++;
    return true;
}

bool Planner::RegisterAction(const std::string& name_, const Action& action_)
{
    bool contains =  (actionCatalogue.find(name_) != actionCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains action \"" + name_ + "\".\n";
        return false;
    }
    actionCatalogue.insert(std::make_pair(std::string(name_), action_ ));
    return true;
}

bool Planner::RegisterAction(const std::string& name_, const WorldState& cnd_, const WorldState& eff_, unsigned cost_)
{
    bool contains =  (actionCatalogue.find(name_) != actionCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains action \"" + name_ + "\".\n";
        return false;
    }
    actionCatalogue.insert(std::make_pair(std::string(name_), Action(cnd_, eff_, cost_)));
    return true;
}

bool Planner::RegisterGoal	(const std::string& name_, const WorldState& goal_)
{
    bool contains = (goalCatalogue.find(name_) != goalCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains goal \"" + name_ + "\".\n";
        return false;
    }
    goalCatalogue.insert(std::make_pair(std::string(name_), goal_ ));
    return true;
}

bool Planner::RegisterGoal(const std::string& name_, const std::unordered_map<std::string, std::string>& nameValuePairs_)
{
    bool contains = (goalCatalogue.find(name_) != goalCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains goal \"" + name_ + "\".\n";
        return false;
    }
    goalCatalogue.insert(std::make_pair(std::string(name_), WorldState(nameValuePairs_)));
    return true;
}

const Attribute& Planner::GetAttribute(const std::string& name_) const
{
    auto search = attributeCatalogue.find(name_);
    if (search == attributeCatalogue.end())
    {
        std::cout << "Attribute \"" + name_ + "\" is not in the catalogue\n";
        return Attribute();
    }
    return search->second;
}

const Action& Planner::GetAction(const std::string& name_) const
{
    auto search = actionCatalogue.find(name_);
    if (search == actionCatalogue.end())
    {
        std::cout << "Action \"" + name_ + "\" is not in the catalogue\n";
        return Action();
    }
    return search->second;
}

const WorldState& Planner::GetGoal(const std::string& name_) const
{
    auto search = goalCatalogue.find(name_);
    if (search == goalCatalogue.end())
    {
        std::cout << "Goal \"" + name_ + "\" is not in the catalogue\n";
        return WorldState();
    }
    return search->second;
}

bool Planner::ConstructPlan(Plan& plan_) const
{
    Path<Vertex> path;
    const auto& goal = GetGoal(plan_.goalName);
    std::set<std::string> availableActionNames;
    for (auto& actionName : actionCatalogue)
        availableActionNames.insert(actionName.first);
    Vertex start(goal, availableActionNames, ""); // we build the path from goal state to current state
    Vertex finish(plan_.startingWs, {}, "");
    bool foundPath = Pathfind(path, start, finish);
    if (foundPath == false)
        return false;
    auto planLength = path.vertices.size() - 1;
    plan_.actionNames.resize(planLength);
    for (auto i = 1; i < path.vertices.size(); i++)
        plan_.actionNames[i - 1] = path.vertices[path.vertices.size() - i].prevAction;
    plan_.cost = path.cost;
    return true;
}

void Planner::GetNeighbors(std::vector<Vertex>& neighbors_, const Vertex& vertex_, const Vertex& finish_) const
{
    for (auto& actionName : vertex_.availableActionNames)
    {
        WorldState nextState; //change state by action
        auto& action = GetAction(actionName);
        if (WorldState::IsActionUseful(nextState, vertex_.state, action)) //check if nextState is closer to finish_ than vertex_.state
            {
            auto neighborAvailableActions = vertex_.availableActionNames;
            neighborAvailableActions.erase(actionName);
            neighbors_.emplace_back(nextState, neighborAvailableActions, actionName);
            }
    }
}

bool Planner::Satisfies(const Vertex& vertex_, const Vertex& targetVertex_) const
{
    const auto& targetState = targetVertex_.state;
    const auto& activeCndSet = vertex_.state;
    return (targetState.GetMask()  & activeCndSet.GetMask()) == activeCndSet.GetMask();
}

t_node_id Planner::GetId(const Vertex& vertex_) const
{
    if (typeid(t_node_id) != typeid(t_mask))
    {
        std::cout << "t_node_id and t_mask have to same types! \n";
        exit(-1);
    }
    return t_node_id(vertex_.state.GetMask());
}

unsigned Planner::GetDist(const Vertex& from_, const Vertex& to_) const
{
    return GetAction(to_.prevAction).GetCost();
}


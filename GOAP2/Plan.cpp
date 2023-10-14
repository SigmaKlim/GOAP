#include "Plan.h"
#include <algorithm>
#include <iostream>
#include <algorithm>
Plan::Plan(const WorldState& startingWs_, const std::string& goalName_, const std::vector<std::string*>& actionNames_) : startingWs(startingWs_), goalName(goalName_), actionNames(actionNames_)
{

}

const WorldState& Plan::GetStartingWs() const
{
    return startingWs;
}

const std::string& Plan::GetGoalName() const
{
    return goalName;
}

const std::vector<std::string*>& Plan::GetActionNames() const
{
    return actionNames;
}


Action Planner::MakeAction() const
{
    return Action(MakeWs(), MakeWs(), 1);
}

std::vector<const std::string*> Planner::GetActionNamesVector() const
{
    std::vector<const std::string*> anVector;
    for (auto& a : actionCatalogue)
        anVector.push_back(&a.first);
    return anVector;
}

Planner::Planner()
{
    goalCatalogue = std::map<std::string, WorldState>();
    actionCatalogue = std::map<std::string, Action>();
    attributeNamesCatalogue = std::vector<std::string>();
}

Planner::~Planner()
{
}

bool Planner::AddAttribute(const std::string& name)
{
    bool contains = (std::find(attributeNamesCatalogue.begin(), attributeNamesCatalogue.end(), name)) != attributeNamesCatalogue.end();
    if (contains)
    {
        std::cout << "The Planner already contains attribute \"" + name + "\".\n";
        return false;
    }
    attributeNamesCatalogue.push_back(name);
    return true;
}

bool Planner::GetGoalByName(WorldState* result, const std::string& name) const
{
    auto it = goalCatalogue.find(name);
    if (it == goalCatalogue.end())
        return false;
    *result = it._Ptr->_Myval.second;
    return true;
}

bool Planner::GetActionByName(Action* result, const std::string& name) const
{
    auto it = actionCatalogue.find(name);
    if (it == actionCatalogue.end())
        return false;
    *result = it._Ptr->_Myval.second;
    return true;
}

bool Planner::AddAction(const std::string& name, const std::map <std::string, bool> cnd, const std::map <std::string, bool> eff, const int cost)
{
    WorldState cndWs = MakeWs();
    if (FillInWs(cndWs, cnd) == false)
        return false;
    WorldState effWs = MakeWs();
    if (FillInWs(effWs, eff) == false)
        return false;
    auto status = actionCatalogue.insert({ name, Action(cndWs, effWs, cost) });
    if (status.second == false)
    {
        std::cout << "Insertion of action \"" + name + "\" into the catalogue failed.\n";
        return false;
    }
    return true;
}

bool Planner::AddGoal(const std::string& name, const std::map<std::string, bool> attrs)
{
    WorldState goalWs = MakeWs();
    if (FillInWs(goalWs, attrs) == false)
        return false;
    auto status = goalCatalogue.insert({ name, goalWs });
    if (status.second == false)
    {
        std::cout << "Insertion of goal \"" + name + "\" into the catalogue failed.\n";
        return false;
    }
    return true;
}




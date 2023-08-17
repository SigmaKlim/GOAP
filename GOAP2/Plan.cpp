#include "Plan.h"
#include <algorithm>
#include <iostream>
#include <algorithm>
Plan::Plan(const WsMask& startingWs_, const std::string& goalName_, const std::vector<std::string*>& actionNames_) : startingWs(startingWs_), goalName(goalName_), actionNames(actionNames_)
{

}

const WsMask& Plan::GetStartingWs() const
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

WsMask Planner::MakeWs() const
{
    return WsMask(ATTRIBUTES_MAX_NUM);
}

bool Planner::FillInWs(WsMask& newWs, const std::map<std::string, bool> attrs) const
{
    std::vector <int> affectedAttributes;
    for (auto const &i : attrs)
    {
        int index = std::find(attributeNamesCatalogue.begin(), attributeNamesCatalogue.end(), i.first) - attributeNamesCatalogue.begin();
        if (index < attributeNamesCatalogue.size())
        {
            newWs.SetAttributeByIndex(index, i.second);
            affectedAttributes.push_back(index);
        }
        else
        {
            std::cout << "Could not find attribute \"" + i.first + "\" in catalogue.\n";
            return false;
        }
    }
    newWs.SetMask(affectedAttributes);
    newWs.affectedAttributesNum = affectedAttributes.size();
    return true;
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
    goalCatalogue = std::map<std::string, WsMask>();
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

bool Planner::GetGoalByName(WsMask* result, const std::string& name) const
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
    WsMask cndWs = MakeWs();
    if (FillInWs(cndWs, cnd) == false)
        return false;
    WsMask effWs = MakeWs();
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
    WsMask goalWs = MakeWs();
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




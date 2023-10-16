#include "Plan.h"
#include <algorithm>
#include <iostream>
#include <algorithm>
Plan::Plan(const WorldState& startingWs_, const std::string& goalName_, const std::vector<std::string>& actionNames_) : startingWs(startingWs_), goalName(goalName_), actionNames(actionNames_)
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

const std::vector<std::string>& Plan::GetActionNames() const
{
    return actionNames;
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

bool Planner::RegisterGoal(const std::string& name_, const WorldState& worldState_)
{
    bool contains = (goalCatalogue.find(name_) != goalCatalogue.end());
    if (contains == true)
    {
        std::cout << "The Planner already contains goal \"" + name_ + "\".\n";
        return false;
    }
    goalCatalogue.insert(std::make_pair(std::string(name_), worldState_ ));
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




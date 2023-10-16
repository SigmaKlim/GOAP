#pragma once
#include "Action.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "Pathfind.hpp"

class Plan
{
	WorldState startingWs;
	std::string goalName;
	std::vector <std::string> actionNames;

public:
	Plan(const WorldState& startingWs_, const std::string& goalName_, const std::vector<std::string>& actionNames_);
	const WorldState& GetStartingWs() const;
	const std::string& GetGoalName() const;
	const std::vector<std::string>& GetActionNames() const;

	friend class Planner;
};

class Planner
{
	std::unordered_map<std::string, WorldState> goalCatalogue;
	std::unordered_map<std::string, Action> actionCatalogue;
	std::unordered_map<std::string, Attribute> attributeCatalogue;

public:

						Planner();
						~Planner();
	bool				RegisterAttribute	(const std::string& name_, const Attribute& attribute_);
	bool				RegisterAttribute	(const std::string& name_, const std::vector<std::string>& enumerators_);
	bool				RegisterAction		(const std::string& name_, const Action& action_);
	bool				RegisterAction		(const std::string& name_, const WorldState& cnd_, const WorldState& eff_, unsigned cost_);
	bool				RegisterGoal		(const std::string& name_, const WorldState& goal_);
	bool				RegisterGoal		(const std::string& name_, const std::unordered_map<std::string, u_char>& nameValuePairs_);
	const Attribute&	GetAttribute		(const std::string& name_) const;
	const Action&		GetAction			(const std::string& name_) const;
	const WorldState&	GetGoal				(const std::string& name_) const;

	const unsigned MAX_ATTRIBUTES = 20;
};


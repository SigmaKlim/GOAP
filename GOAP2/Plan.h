#pragma once
#include "Action.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "Pathfind.hpp"
#include "Pathfind.hpp"
struct Vertex
{
	Vertex					()  = default;
	Vertex					(const WorldState& state_, const std::set <std::string>& availableActionNames_, const std::string& prevAction_) :
								availableActionNames(availableActionNames_), state(state_), prevAction(prevAction_) {};

	std::set<std::string> availableActionNames;
	WorldState state;
	std::string prevAction;
};

struct Plan
{
	WorldState startingWs;
	std::string goalName;
	const std::vector<std::string>& GetActionSequence() const;
	unsigned GetCost() const;
private:
	std::vector <std::string> actionNames;
	unsigned cost;

	friend class Planner;
};

class Planner : public BasePathfinder<Vertex>
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
	bool				RegisterGoal		(const std::string& name_, const std::unordered_map<std::string, std::string>& nameValuePairs_);
	const Attribute&	GetAttribute		(const std::string& name_) const;
	const Action&		GetAction			(const std::string& name_) const;
	const WorldState&	GetGoal				(const std::string& name_) const;

	bool ConstructPlan(Plan& plan_) const;
	
	void		GetNeighbors(std::vector<Vertex>& neighbors_, const Vertex& vertex_, const Vertex& finish_ = Vertex()) const override;
	bool		Satisfies	(const Vertex& vertex_, const Vertex& finish_ = Vertex()) const override;
	t_node_id	GetId		(const Vertex& vertex_) const override;
	unsigned	GetDist		(const Vertex& from_, const Vertex& to_) const override;
	const unsigned MAX_ATTRIBUTES = 20;
};


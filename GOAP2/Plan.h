#pragma once
#include "Action.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "Pathfind.hpp"
#include "BitMask.h"
struct Vertex
{
	Vertex	()  = default;
	Vertex	(const WorldState& state_, const std::set <std::string>& availableActionNames_, const std::string& prevAction_) :
					availableActionNames(availableActionNames_), state(state_), prevAction(prevAction_) {};

	std::set<std::string> availableActionNames;
	WorldState state;
	std::string prevAction;
};

struct Plan
{
	WorldState StartingWs;
	std::string GoalName;
	const std::vector<std::string>& GetActionSequence() const;
	unsigned GetCost() const;
private:
	std::vector <std::string> _actionNames;
	unsigned _cost = 0;


	friend class Planner;
};
template <typename t_vertex, typename t_id>
class Planner : public BasePathfinder<t_vertex, t_id> {};

template <>
class Planner 
{
	std::unordered_map<std::string, WorldState> _goalCatalogue;
	std::unordered_map<std::string, Action> _actionCatalogue;
	std::unordered_map<std::string, Attribute> _attributeCatalogue;


public:

						Planner();
						~Planner();
	bool				RegisterAttribute	(const std::string& name, const Attribute& attribute);
	bool				RegisterAttribute	(const std::string& name, const std::vector<std::string>& enumerators);
	bool				RegisterAction		(const std::string& name, const Action& action);
	bool				RegisterAction		(const std::string& name, const WorldState& cnd, const WorldState& eff, unsigned cost);
	bool				RegisterGoal		(const std::string& name, const WorldState& goal_);
	bool				RegisterGoal		(const std::string& name, const std::unordered_map<std::string, std::string>& nameValuePairs);
	const Attribute&	GetAttribute		(const std::string& name) const;
	const Action&		GetAction			(const std::string& name) const;
	const WorldState&	GetGoal				(const std::string& name) const;

	bool ConstructPlan(Plan& plan_) const;
	
private:
	void		GetNeighbors(std::vector<Vertex>& neighbors, const Vertex& vertex, const Vertex& finish = Vertex()) const override;
	//Returns true of the action fulfills any of active conditions

	bool		Satisfies	(const Vertex& vertex, const Vertex& targetVertex = Vertex()) const override;
	BitMask		GetId		(const Vertex& vertex) const override;
	unsigned	GetDist		(const Vertex& from, const Vertex& to) const override;
	static const unsigned MAX_ATTRIBUTES = std::numeric_limits<unsigned>::digits;
;
};


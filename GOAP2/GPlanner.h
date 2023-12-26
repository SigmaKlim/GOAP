#pragma once
#include "Action.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "AStar.h"
#include "BitMask.h"
#include "ActionInputDataBase.h"

struct Vertex
{
	Vertex	()  = default;
	Vertex	(const WorldState& state_, const std::set <std::string>& availableActionNames_, const std::string& prevAction_) :
					availableActionNames(availableActionNames_), activeConditionSet(state_), prevAction(prevAction_) {};

	std::set<std::string> availableActionNames;
	WorldState activeConditionSet; //A set of conditions required for all previously taken actions
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


	friend class GPlanner;
};


class GPlanner : public AStarSolver<Vertex, BitMask>
{
public:

						GPlanner			();
						~GPlanner			() override = default;
	bool				RegisterAttribute	(const std::string& name, const Attribute& attribute);
	bool				RegisterAttribute	(const std::string& name, const std::vector<std::string>& enumerators);
	bool				RegisterAction		(const std::string& name, const Action& action);
	bool				RegisterAction		(const std::string& name, const WorldState& cnd, const WorldState& eff, unsigned cost);
	bool				RegisterAction		(const std::string& name, const WorldState& cnd, const std::set<std::string>& affectedAttributes, WorldState (*EvaluateEffect) (ActionInputDataBase* data), unsigned (*CalculateCost) (ActionInputDataBase* data));
	bool				RegisterGoal		(const std::string& name, const WorldState& goal_);
	bool				RegisterGoal		(const std::string& name, const std::unordered_map<std::string, std::string>& nameValuePairs);
	const Attribute&	GetAttribute		(const std::string& name) const;
	const Action&		GetAction			(const std::string& name) const;
	const WorldState&	GetGoal				(const std::string& name) const;
	bool				ConstructPlan		(Plan& plan, TelemetryData* telemetryData = nullptr, void* userData = nullptr) const;
	
	const unsigned MAX_ATTRIBUTES = std::numeric_limits<unsigned>::digits;

private:
	
	void GetNeighbors (std::vector<Vertex>&	neighbors, const Vertex& vertex, const Vertex& finish = Vertex(), void* userData = nullptr) const override;
	bool Satisfies (const Vertex& vertex, const Vertex& targetVertex = Vertex(), void* userData = nullptr) const override;
	BitMask	GetId (const Vertex& vertex) const override;
	unsigned GetDistance (const Vertex& from, const Vertex& to, void* userData) const override;
	//A simple heuristic of how many attributes from initial state do not satisfy conditions from the inspected vertex
	unsigned GetHeuristic(const Vertex& vertex, const Vertex& target, void* userData = nullptr) const override;

	bool IsActionUseful	 (WorldState& modifiedConditionSet, const WorldState& conditionSet, const Action& action, ActionInputDataBase* actionData) const;
	std::unordered_map<std::string, WorldState> _goalCatalogue;
	std::unordered_map<std::string, Action>		_actionCatalogue;
	std::unordered_map<std::string, Attribute> _attributeCatalogue;
};







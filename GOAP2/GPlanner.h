#pragma once
#include "Actions/IAction.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "AStar.h"
#include "BitMask.h"

struct Vertex
{
	Vertex	()  = default;
	Vertex	(const WorldState& state, const std::set <std::string>& availableActionNames, const std::string& prevActionName) :
					AvailableActionNames(availableActionNames), ActiveConditionSet(state), PrevActionName(prevActionName){}

	std::set<std::string> AvailableActionNames;
	WorldState ActiveConditionSet; //A set of conditions required for all previously taken actions
	std::string PrevActionName;
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
	bool				RegisterAction		(const std::string& name, IAction& action);
	bool				RegisterGoal		(const std::string& name, const WorldState& goal_);
	bool				RegisterGoal		(const std::string& name, const std::unordered_map<std::string, std::string>& nameValuePairs);
	const Attribute&	GetAttribute		(const std::string& name) const;
	const IAction&		GetAction			(const std::string& name) const;
	const WorldState&	GetGoal				(const std::string& name) const;
	bool				ConstructPlan		(Plan& plan, TelemetryData* telemetryData = nullptr, void* userData = nullptr) const;
	
	const std::unordered_map<std::string, Attribute>& GetAttributeCatalogue() const;
	const unsigned MAX_ATTRIBUTES = std::numeric_limits<unsigned>::digits;

private:
	
	void GetNeighbors (std::vector<Vertex>&	neighbors, const Vertex& vertex, const Vertex& finish = Vertex()) const override;
	bool Satisfies (const Vertex& vertex, const Vertex& targetVertex = Vertex()) const override;
	BitMask	GetId (const Vertex& vertex) const override;
	unsigned GetDistance (const Vertex& from, const Vertex& to) const override;
	//A simple heuristic of how many attributes from initial state do not satisfy conditions from the inspected vertex
	unsigned GetHeuristic(const Vertex& vertex, const Vertex& target, void* userData = nullptr) const override;

	//The function checks if the action satisfies any conditions from the set and does not violate any specific rules. If so, returns
	//true, evaluates resulted condition set after the action has been included in the plan and sets modifiedConditionSet to it.
	//Otherwise, returns false.
	bool IsActionUseful	 (WorldState& modifiedConditionSet, const WorldState& conditionSet, const IAction& action) const;
	std::unordered_map<std::string, WorldState> _goalCatalogue;
	std::unordered_map<std::string, IAction&>	_actionCatalogue;
	std::unordered_map<std::string, Attribute> _attributeCatalogue;
};







#pragma once
#include "Actions/IAction.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "AStar.h"
#include "BitMask.h"
#include "Tools\Catalogue.h"
struct Vertex
{
	Vertex	()  = default;
	Vertex	(const WorldState& state, /*const std::set <std::size_t>& availableActionIds,*/ size_t prevActionId) :
					/*AvailableActionIds(availableActionIds),*/ ActiveConditionSet(state), PrevActionId(prevActionId){}

	//std::set<size_t> AvailableActionIds;
	WorldState ActiveConditionSet; //A set of conditions required for all previously taken actions
	std::size_t PrevActionId;
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
	template<typename t_action>
	bool				RegisterAction		(const std::string& name, const t_action& action);
	//bool				RegisterAction		(const std::string& name, IAction& action);
	bool				RegisterGoal		(const std::string& name, const WorldState& goal_);
	bool				RegisterGoal		(const std::string& name, const std::unordered_map<std::string, std::string>& nameValuePairs);
	const Attribute&	GetAttribute		(const std::string& name) const;
						const IAction* GetAction(size_t id) const;
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
	Catalogue<const IAction*> _actionCatalogue;
	//std::unordered_map<std::string, IAction&>	_actionCatalogue;
	std::unordered_map<std::string, Attribute> _attributeCatalogue;
};

//Copy you action object to planner's memory. Object type must support IAction interface and have copy-constructor.
template <typename t_action>
bool GPlanner::RegisterAction(const std::string& name, const t_action& action)
{
	if (dynamic_cast<const IAction*>(&action) == nullptr)
	{
		std::cout << "Failed to register action " << name <<
			". This type does not implement IAction interface.\n";
		return false;	
	}
	IAction* iaction = new t_action(action);
	return _actionCatalogue.AddItem(name, iaction);
}

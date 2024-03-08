#pragma once
#include "Actions/Action.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "AStar.h"
#include "BitMask.h"
#include "Tools\Catalogue.h"
struct Vertex
{
	Vertex	()  = default;
	Vertex	(const WorldState& state, size_t prevActionId) : ActiveConditionSet(state), PrevActionId(prevActionId){}
	Vertex	(const WorldState& state, size_t prevActionId, const std::string& prevActionName) : ActiveConditionSet(state), PrevActionId(prevActionId), PrevActionName(prevActionName){}

	WorldState ActiveConditionSet; //A set of conditions required for all previously taken actions
	std::size_t PrevActionId;
	//debug
	std::string PrevActionName; 
};

struct Plan
{
	WorldState StartingWs;
	std::string GoalName;
	const std::vector<std::string>& GetActionSequence() const;
	float GetCost() const;
private:
	std::vector <std::string> _actionNames;
	float _cost = 0;


	friend class GPlanner;
};


class GPlanner : public AStarSolver<Vertex, BitMask>
{
public:

						GPlanner			();
						~GPlanner			() override = default;

	template <typename t_attribute>
	bool RegisterAttribute(const std::string& name, const t_attribute& attribute);

	template<typename t_action>
	bool				RegisterAction		(const std::string& name, const t_action& action);

	bool				RegisterGoal		(const std::string& name, const WorldState& goal_);

	const Action*		GetAction(size_t id) const;
	const WorldState&	GetGoal				(const std::string& name) const;

	bool				ConstructPlan		(Plan& plan, TelemetryData* telemetryData = nullptr, void* userData = nullptr) const;

	const Catalogue<const Attribute*>&			GetAttributeCatalogue() const;
	
	const unsigned	MAX_ATTRIBUTES = std::numeric_limits<unsigned>::digits;

	float GetDistanceDenominator()		const override;
	float GetHeuristicsDenominator()	const override;
private:
	
	void				GetNeighbors (std::vector<Vertex>&	neighbors, const Vertex& vertex, const Vertex& finish = Vertex()) const override;
	bool				Satisfies (const Vertex& vertex, const Vertex& targetVertex = Vertex()) const override;
	BitMask				GetId (const Vertex& vertex) const override;
	float				GetDistance(const Vertex& from, const Vertex& to) const override;
	//A simple heuristic of how many attributes from initial state do not satisfy conditions from the inspected vertex
	float				GetHeuristic(const Vertex& vertex, const Vertex& target, void* userData = nullptr) const override;


	//The function checks if the action satisfies any conditions from the set and does not violate any specific rules. If so, returns
	//true, evaluates resulted condition set after the action has been included in the plan and sets modifiedConditionSet to it.
	//Otherwise, returns false.
	bool				IsActionUseful	 (WorldState& modifiedConditionSet, const WorldState& conditionSet, const Action& action) const;
	
	std::unordered_map<std::string, WorldState> _goalCatalogue;
	Catalogue<const Action*>					_actionCatalogue;
	Catalogue<const Attribute*>					_attributeCatalogue;
	float _totalActionMaxCost = 0.0f;			// sum of all action max costs
	float _totalAttributeMaxDistance = 0.0f;	//sum of all attribute max distances
};

template <typename t_attribute>
bool GPlanner::RegisterAttribute(const std::string& name, const t_attribute& attribute)
{
	if (dynamic_cast<const Attribute*>(&attribute) == nullptr)
	{
		std::cout << "Failed to register attribute " << name <<
			". This type is not derived from Attribute class.\n";
		return false;	
	}
	WorldState::_numAttributes++;
	bool result = _attributeCatalogue.AddItem(name, new t_attribute(attribute));
	if (result == false)
		return false;
	_totalAttributeMaxDistance += dynamic_cast<const Attribute*>(&attribute)->GetMaxDifference();
	return true;
}

//Copy you action object to planner's memory. Object type must support IAction interface and have copy-constructor.
template <typename t_action>
bool GPlanner::RegisterAction(const std::string& name, const t_action& action)
{
	if (dynamic_cast<const Action*>(&action) == nullptr)
	{
		std::cout << "Failed to register action " << name <<
			". This type is not derived from Attribute class.\n";
		return false;	
	}
	bool result = _actionCatalogue.AddItem(name, new t_action(action));
	if (result == false)
		return false;
	_totalActionMaxCost += dynamic_cast<const Action*>(&action)->GetHighestPossibleCost();
	return true;
}
#pragma once
#include "Action.h"
#include <string>
#include <vector>
#include <map>
#include "Pathfind.hpp"

class Plan
{
	WorldState startingWs;
	std::string goalName;
	std::vector <std::string*> actionNames;

public:
	Plan(const WorldState& startingWs_, const std::string& goalName_, const std::vector<std::string*>& actionNames_);
	const WorldState& GetStartingWs() const;
	const std::string& GetGoalName() const;
	const std::vector<std::string*>& GetActionNames() const;

	friend class Planner;
};

class Planner
{
	std::map<std::string, WorldState> goalCatalogue;
	std::map<std::string, Action> actionCatalogue;
	std::vector<std::string*> idToAction;
	std::vector<std::string> attributeNamesCatalogue;
	

	const int ATTRIBUTES_MAX_NUM = 20;

public:
	Action MakeAction() const;
	std::vector <const std::string*> GetActionNamesVector() const;
	//public:
	Planner();
	~Planner();
	bool AddAttribute(const std::string& name);
	bool GetGoalByName(WorldState* result, const std::string& name) const;
	bool GetActionByName(Action* result, const std::string& name) const;
	bool AddAction(const std::string& name, const std::map <std::string, bool> cnd, const std::map <std::string, bool> eff, const int cost);
	bool AddGoal(const std::string& name, const WorldState& worldState);
};

struct Vertex
{
	std::vector<const std::string*> availableActions;
	WorldState state;
};

class GoapPathfinder : public BasePathfinder <Vertex>
{
	Planner& planer;
public:
	GoapPathfinder();
	std::vector <std::pair<node_id, arc_id>> GetNeighbors(const node_id id) const;
	int GetDist(const arc_id& arcId) const;
	bool Satisfies(const Node<Vertex>* node, const Node<Vertex>* target) const;
};
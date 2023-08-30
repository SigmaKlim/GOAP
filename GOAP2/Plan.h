#pragma once
#include "Action.h"
#include <string>
#include <vector>
#include <map>
#include "Pathfind.hpp"

class Plan
{
	WsMask startingWs;
	std::string goalName;
	std::vector <std::string*> actionNames;

public:
	Plan(const WsMask& startingWs_, const std::string& goalName_, const std::vector<std::string*>& actionNames_);
	const WsMask& GetStartingWs() const;
	const std::string& GetGoalName() const;
	const std::vector<std::string*>& GetActionNames() const;

	friend class Planner;
};

class Planner
{
	std::map<std::string, WsMask> goalCatalogue;
	std::map<std::string, Action> actionCatalogue;
	std::vector<std::string*> idToAction;
	std::vector<std::string> attributeNamesCatalogue;
	

	const int ATTRIBUTES_MAX_NUM = 20;

public:
	WsMask MakeWs() const;
	bool FillInWs(WsMask& emptyWs, const std::map <std::string, bool> attrs) const;
	Action MakeAction() const;
	std::vector <const std::string*> GetActionNamesVector() const;
	//public:
	Planner();
	~Planner();
	bool AddAttribute(const std::string& name);
	bool GetGoalByName(WsMask* result, const std::string& name) const;
	bool GetActionByName(Action* result, const std::string& name) const;
	bool AddAction(const std::string& name, const std::map <std::string, bool> cnd, const std::map <std::string, bool> eff, const int cost);
	bool AddGoal(const std::string& name, const std::map <std::string, bool> attrs);
};

struct Vertex
{
	std::vector<const std::string*> availableActions;
	//std::vector<const std::string*> path;
	WsMask cnd;
	WsMask state;
};

class GoapPathfinder : public BasePathfinder <Vertex>
{

public:
	GoapPathfinder();
	std::vector <std::pair<node_id, arc_id>> GetNeighbors(const node_id id) const;
	int GetDist(const arc_id& arcId) const;
	bool Satisfies(const Node<Vertex>* node, const Node<Vertex>* target) const;
};
#pragma once
#include "World.h"

class Action
{
	WorldState cnd;
	WorldState eff;
	int cost;


public:
	Action(const WorldState& cnd_, const WorldState& eff_, const int cost_);
	~Action();

	const WorldState& GetCnd() const;
	const WorldState& GetEff() const;
	int GetCost() const;
	void SetCost(const int cost_);

	friend class Planner;
};
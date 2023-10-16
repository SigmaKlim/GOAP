#pragma once
#include "World.h"

class Action
{
	WorldState cnd;
	WorldState eff;
	unsigned cost;

public:
	Action() = default;
	Action(const WorldState& cnd_, const WorldState& eff_, const unsigned cost_);
	~Action();

	const WorldState& GetCnd() const;
	const WorldState& GetEff() const;
	unsigned GetCost() const;
	void SetCost(const int cost_);

	friend class Planner;
};
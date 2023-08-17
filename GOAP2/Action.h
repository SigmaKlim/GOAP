#pragma once
#include "World.h"

class Action
{
	WsMask cnd;
	WsMask eff;
	int cost;


public:
	Action(const WsMask& cnd_, const WsMask& eff_, const int cost_);
	~Action();

	const WsMask& GetCnd() const;
	const WsMask& GetEff() const;
	int GetCost() const;
	void SetCost(const int cost_);

	friend class Planner;
};
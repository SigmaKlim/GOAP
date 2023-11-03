#pragma once
#include "World.h"

class Action
{
	WorldState _condition;
	WorldState _effect;
	unsigned _cost;

public:
						Action() = default;
						Action(const WorldState& cnd_, const WorldState& eff_, const unsigned cost_);
						~Action();

	const WorldState&	GetConditions() const; 
	const WorldState&	GetEffects() const;
	unsigned			GetCost() const;
	void				SetCost(const int cost_);
	
	template <typename t_vertex, typename t_id>
	friend class AStartPathfinder;
	friend struct WorldState;
};
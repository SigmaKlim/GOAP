#include "Action.h"

Action::Action(const WorldState& cnd_, const WorldState& eff_, const unsigned cost_) : _condition(cnd_), _effect(eff_), _cost(cost_)
{
	
}

Action::~Action()
{
}

const WorldState& Action::GetConditions() const
{
	return _condition;
}

const WorldState& Action::GetEffects() const
{
	return _effect;
}

unsigned Action::GetCost() const
{
	return _cost;
}

void Action::SetCost(const int cost_)
{
	_cost = cost_;
}

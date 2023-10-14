#include "Action.h"

Action::Action(const WorldState& cnd_, const WorldState& eff_, const int cost_) : cnd(cnd_), eff(eff_), cost(cost_)
{
	
}

Action::~Action()
{
}

const WorldState& Action::GetCnd() const
{
	return cnd;
}

const WorldState& Action::GetEff() const
{
	return eff;
}

int Action::GetCost() const
{
	return cost;
}

void Action::SetCost(const int cost_)
{
	cost = cost_;
}

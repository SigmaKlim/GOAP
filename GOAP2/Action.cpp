#include "Action.h"

Action::Action(const WsMask& cnd_, const WsMask& eff_, const int cost_) : cnd(cnd_), eff(eff_), cost(cost_)
{
	
}

Action::~Action()
{
}

const WsMask& Action::GetCnd() const
{
	return cnd;
}

const WsMask& Action::GetEff() const
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

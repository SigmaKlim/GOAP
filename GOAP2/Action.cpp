#include "Action.h"

Action::Action(const WorldState& condition, const WorldState& effect, unsigned cost) : _condition(condition), _effect(effect), _cost(cost)
{
	
}

Action::Action(const WorldState& condition, const BitMask& affectedAttributeMask, WorldState(* EvaluateEffect)(ActionInputDataBase* data), unsigned cost) : _condition(condition), EvaluateEffect(EvaluateEffect), _cost(cost)
{
	_effect._affectedAttributesMask = affectedAttributeMask;
}

Action::Action(const WorldState& condition, const WorldState& effect, unsigned(* CalculateCost)(ActionInputDataBase* data)) : _condition(condition), _effect(effect), CalculateCost(CalculateCost)
{
}

Action::Action(const WorldState& condition, const BitMask& affectedAttributeMask, WorldState(* EvaluateEffect)(ActionInputDataBase* data),
	unsigned(* CalculateCost)(ActionInputDataBase* data)) : _condition(condition), EvaluateEffect(EvaluateEffect), CalculateCost(CalculateCost)
{
	_effect._affectedAttributesMask = affectedAttributeMask;
}


const WorldState& Action::GetConditions() const
{
	return _condition;
}

WorldState Action::GetOrEvaluateEffect(ActionInputDataBase* data) const
{
	if (EvaluateEffect == nullptr)
		return _effect;
	return EvaluateEffect(data);
}

unsigned Action::GetOrCalculateCost(ActionInputDataBase* data) const
{
	if (CalculateCost == nullptr)
		return _cost;
	return CalculateCost(data);
}


void Action::SetCost(int cost)
{
	_cost = cost;
}

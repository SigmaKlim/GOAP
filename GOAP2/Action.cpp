#include "Action.h"

Action::Action(const WorldState& condition, const WorldState& effect, unsigned cost) : _condition(condition), _effect(effect), _cost(cost)
{
	
}

Action::Action(const WorldState& condition, WorldState(* EvaluateEffect)(const WorldState& desiredState, void* data), unsigned cost) : _condition(condition), EvaluateEffect(EvaluateEffect), _cost(cost)
{
}

Action::Action(const WorldState& condition, const WorldState& effect, unsigned(* CalculateCost)(void* data)) : _condition(condition), _effect(effect), CalculateCost(CalculateCost)
{
}

Action::Action(const WorldState& condition, WorldState(* EvaluateEffect)(const WorldState& desiredState, void* data), unsigned(* CalculateCost)(void* data)): _condition(condition), EvaluateEffect(EvaluateEffect), CalculateCost(CalculateCost)
{
}

const WorldState& Action::GetConditions() const
{
	return _condition;
}

WorldState Action::GetOrEvaluateEffect(const WorldState& desiredState, void* data) const
{
	if (EvaluateEffect == nullptr)
		return _effect;
	return EvaluateEffect(desiredState, data);
}

unsigned Action::GetOrCalculateCost(void* data) const
{
	if (CalculateCost == nullptr)
		return _cost;
	return CalculateCost(data);
}


void Action::SetCost(int cost)
{
	_cost = cost;
}

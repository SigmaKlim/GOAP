#pragma once
#include "World.h"
#include "ActionInputDataBase.h"
class Action
{
	WorldState _condition;
	WorldState _effect;
	//A function to evaluate effects of the action, in case it depends on outer parameters. In other case, you should use constant _effect member.
	WorldState (*EvaluateEffect) (ActionInputDataBase* data) = nullptr; 
	unsigned _cost = 0;
	//A function to evaluate cost of the action, in case it depends on outer parameters. In other case, you should use constant _cost member.
	unsigned (*CalculateCost) (ActionInputDataBase* data) = nullptr;
	
public:
						Action() = default;
						Action(const WorldState& condition, const WorldState& effect, unsigned cost);
						Action(const WorldState& condition, const BitMask& affectedAttributeMask, WorldState (*EvaluateEffect) (ActionInputDataBase* data), unsigned cost);
						Action(const WorldState& condition, const WorldState& effect, unsigned (*CalculateCost) (ActionInputDataBase* data));
						Action(const WorldState& condition, const BitMask& affectedAttributeMask, WorldState (*EvaluateEffect) (ActionInputDataBase* data),	unsigned (*CalculateCost) (ActionInputDataBase* data));
						~Action() = default;

	const WorldState&	GetConditions() const;
	//If EvaluateEffect == nullptr, returns _effect, otherwise returns EvaluateEffect(desiredState, data)
	WorldState			GetOrEvaluateEffect(ActionInputDataBase* data) const;
	//If CalculateCost == nullptr, returns _cost, otherwise returns CalculateCost(data)
	unsigned			GetOrCalculateCost(ActionInputDataBase* data) const;
	void				SetCost(int cost);
	
	
	friend class GPlanner;
	friend struct WorldState;
};
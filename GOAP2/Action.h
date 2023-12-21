#pragma once
#include "World.h"

class Action
{
	WorldState _condition;
	WorldState _effect;
	//A function to evaluate effects of the action, in case it depends on outer parameters. In other case, you should use constant _effect member.
	WorldState (*EvaluateEffect) (const WorldState& desiredState, void* data) = nullptr; 
	unsigned _cost = 0;
	//A function to evaluate cost of the action, in case it depends on outer parameters. In other case, you should use constant _cost member.
	unsigned (*CalculateCost) (void* data) = nullptr;
	
public:
						Action() = default;
						Action(const WorldState& condition, const WorldState& effect,													unsigned cost);
						Action(const WorldState& condition, WorldState (*EvaluateEffect) (const WorldState& desiredState, void* data),	unsigned cost);
						Action(const WorldState& condition, const WorldState& effect,													unsigned (*CalculateCost) (void* data));
						Action(const WorldState& condition, WorldState (*EvaluateEffect) (const WorldState& desiredState, void* data),	unsigned (*CalculateCost) (void* data));
						~Action() = default;

	const WorldState&	GetConditions() const;
	//If EvaluateEffect == nullptr, returns _effect, otherwise returns EvaluateEffect(desiredState, data)
	WorldState			GetOrEvaluateEffect(const WorldState& desiredState = WorldState(), void* data = nullptr) const;
	//If CalculateCost == nullptr, returns _cost, otherwise returns CalculateCost(data)
	unsigned			GetOrCalculateCost(void* data = nullptr) const;
	void				SetCost(int cost);
	
	template <typename t_vertex, typename t_id>
	friend class AStarPathfinder;
	friend struct WorldState;
};
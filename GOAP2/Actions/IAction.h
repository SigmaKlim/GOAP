#pragma once
#include "../World.h"
#include "InputData.h"
class IAction
{
public:
						IAction() = default;
	virtual 			~IAction() = default;

	virtual WorldState	GetCondition() const = 0;
	//Evaluates (if necessary) the effect of the action and returns it
	virtual WorldState	GetEffect(EvaluateActionEffectInputBase* data = nullptr) const = 0;
	//Calculates (if necessary) the cost of the action and returns it.
	virtual unsigned	GetCost(CalculateActionCostInputBase* data = nullptr) const = 0;
	//Returns calculated string. Useful for actions with customised effect/cost evaluation.
	virtual std::string GetPostfixName(const WorldState& desiredState) const
	{
		return "";
	}
};
#pragma once
#include <iostream>

#include "../World/World.h"
#include "InputData.h"
class Action
{
public:
						Action() = default;
	virtual 			~Action() = default;

	virtual WorldState	GetCondition(const WorldState& preState) const = 0;
	//Evaluates (if necessary) the effect of the action and returns it
	virtual WorldState	GetEffect(const WorldState& preState) const = 0;
	//Calculates (if necessary) the cost of the action and returns it.
	virtual float GetCost(const WorldState& preState) const = 0;
	//Returns calculated string. Useful for actions with customised effect/cost evaluation.
	virtual std::string GetEffectPostfix(const WorldState& desiredState) const
	{
		return "";
	}
	//Returns the highest cost value possible for this action required for normalizing dist value in AStar
	virtual float GetHighestPossibleCost() const = 0;
	//Overload this, if your world states use custom WsUserData struct
	virtual WsUserData* ModifyUserData(const WorldState& preState) const
	{
		if (preState.UserData == nullptr)
		{
			std::cout << "UserData was nullptr. Perhaps, you have not set it when creating the start state\n";
			assert(false);
		}
		return new WsUserData(*preState.UserData);
	}
};
#pragma once
#include "../Condition/ConditionSet.h"
#include "../ValueSet.h"
#include "ActionData.h"
//A class for evaluated instances of action
struct Action
{
    ConditionSet Conditions;
    ValueSet Effects;
    float Cost;

    ActionData UserData; //used for constructing new vertices
};

//An interface with methods for constructing action instances from required conditions and user data
class IActionConstructor
{
public:
    virtual ~IActionConstructor() = default;
    
    virtual void ConstructActions(std::vector<Action>& actions, const ConditionSet& requiredConditions, const ActionData& userData) = 0;

    //Returns maximal possible cost for this action
    virtual float GetMaxCost() const = 0;
};


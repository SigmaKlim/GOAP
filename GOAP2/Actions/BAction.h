#pragma once
#include "../Condition/ConditionSet.h"
#include "../WorldMask.h"

struct BAction
{
    virtual ~BAction() = default;
    virtual ConditionSet GetConditions() const = 0;
    virtual WorldMask GetEffects() const = 0;
    virtual float GetCost() const = 0;

    //Returns maximal possible cost for this action
    virtual float GetMaxCost() const = 0;
};


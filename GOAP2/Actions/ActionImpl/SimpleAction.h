#pragma once
#include "../BAction.h"

struct SimpleAction : public BAction
{
    ConditionSet GetConditions() const override
    {
        return Conditions;
    }

    WorldMask GetEffects() const override
    {
        return Effects;
    }
    float GetCost() const override
    {
        return Cost;
    }
    
    ConditionSet Conditions;
    WorldMask Effects;
    float Cost;
};
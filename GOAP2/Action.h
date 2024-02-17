#pragma once
#include "Condition/ConditionSet.h"
#include "WorldMask.h"

struct Action
{
    ConditionSet Conditions;
    WorldMask Effects;
    float Cost = 0;
};
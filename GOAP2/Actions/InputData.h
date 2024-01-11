#pragma once
#include "BitMask.h"
struct EvaluateActionEffectInputBase
{
    const WorldState* DesiredStateMask = nullptr; //value mask of the condition the action is trying to satisfy
};

struct CalculateActionCostInputBase
{
    const WorldState* prevState = nullptr;
};
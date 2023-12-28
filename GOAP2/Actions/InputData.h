#pragma once
#include "BitMask.h"
struct EvaluateActionEffectInputData
{
    const WorldState* DesiredStateMask = nullptr; //value mask of the condition the action is trying to satisfy
};

struct CalculateActionCostInputData
{
    const WorldState* prevState = nullptr;
};
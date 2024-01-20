#pragma once
struct EvaluateActionEffectInputBase
{
    const WorldState* DesiredStateMask = nullptr; //value mask of the condition the action is trying to satisfy
};

struct CalculateActionCostInputBase
{
    const WorldState* postState = nullptr; // the state after the action has been taken
};
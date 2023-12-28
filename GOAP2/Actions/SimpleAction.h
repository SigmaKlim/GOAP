#pragma once
#include "IAction.h"

struct SimpleAction : IAction
{
    WorldState  _condition;
    WorldState  _effect;
    unsigned    _cost;
    SimpleAction() = default;
    SimpleAction(const WorldState& condition, const WorldState& effect, unsigned cost)
    {
        _condition = condition;
        _effect = effect;
        _cost = cost;
    }
    WorldState GetCondition() const override
    {
        return _condition;
    }
    WorldState GetEffect(const EvaluateActionEffectInputData* data) const override
    {
        return _effect;
    }
    unsigned GetCost(const CalculateActionCostInputData* data) const override
    {
        return _cost;
    }

    
};
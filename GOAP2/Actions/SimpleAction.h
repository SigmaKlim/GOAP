#pragma once
#include "Action.h"

struct SimpleAction : public Action
{
    const WorldState    _condition;
    const WorldState    _effect;
    const float         _cost;
    //SimpleAction() = default;
    SimpleAction(const WorldState& condition, const WorldState& effect, float cost) : _condition(condition), _effect(effect), _cost(cost) {}
    
    WorldState GetCondition() const override
    {
        return _condition;
    }
    WorldState GetEffect(EvaluateActionEffectInputBase* data) const override
    {
        return _effect;
    }

    float GetCost(CalculateActionCostInputBase* data) const override
    {
        return _cost;
    }

    float GetHighestPossibleCost() const override
    {
        return _cost;
    }
    
    
};
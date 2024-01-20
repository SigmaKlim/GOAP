#pragma once
#include "Action.h"
#include "../World/MyWsData.h"

struct SimpleAction : public Action
{
    const WorldState    _condition;
    const WorldState    _effect;
    const float         _cost;
    //SimpleAction() = default;
    SimpleAction(const WorldState& condition, const WorldState& effect, float cost) : _condition(condition), _effect(effect), _cost(cost) {}
    
    WorldState GetCondition(const WorldState& preState) const override
    {
        return _condition;
    }
    WorldState GetEffect(const WorldState& preState) const override
    {
        return _effect;
    }

    float GetCost(const WorldState& preState) const override
    {
        return _cost;
    }

    float GetHighestPossibleCost() const override
    {
        return _cost;
    }
    
    WsUserData* ModifyUserData(const WorldState& preState) const override
    {
        if (preState.UserData == nullptr)
        {
            std::cout << "UserData was nullptr. Perhaps, you have not set it when creating the start state\n";
            assert(false);
        }
        auto d = dynamic_cast<MyWsData*>(preState.UserData.get());
        if (d == nullptr)
            return new MyWsData;
        return new MyWsData(*d);
    }
};
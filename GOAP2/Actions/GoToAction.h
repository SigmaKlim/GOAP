#pragma once
#include "Action.h"

class Navigator;
struct GoToAction : Action
{
    //we assume that there are no complex conditions on 'location' attribute!!!
    WorldState _condition;
private:
    Navigator& _navigator;
public:
    const std::string locationAttributeName = "location";
    const std::string enemyStatusAttributeName = "enemyStatus";
    
    GoToAction(Navigator& navigator, const WorldState& condition);

    WorldState  GetCondition()                                          const override;
    WorldState  GetEffect(EvaluateActionEffectInputBase* data)    const override;
    unsigned    GetCost(CalculateActionCostInputBase* data)       const override;
    std::string GetPostfixName(const WorldState& desiredState)          const override;
};
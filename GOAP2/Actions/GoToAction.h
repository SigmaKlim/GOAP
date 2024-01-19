#pragma once
#include "Action.h"

class NavPathfinder;
struct GoToAction : Action
{
    //we assume that there are no complex conditions on 'atPoint' attribute!!!
    WorldState _condition;
private:
    NavPathfinder& _navPathfinder;
public:
    const std::string atPointAttributeName = "atPoint";
    const std::string enemyStatusAttributeName = "enemyStatus";
    
    GoToAction(NavPathfinder& navPathfinder, const WorldState& condition);

    WorldState  GetCondition()                                          const override;
    WorldState  GetEffect(EvaluateActionEffectInputBase* data)    const override;
    unsigned    GetCost(CalculateActionCostInputBase* data)       const override;
    std::string GetPostfixName(const WorldState& desiredState)          const override;
};
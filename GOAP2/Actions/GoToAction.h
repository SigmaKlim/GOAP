#pragma once
#include "Action.h"

struct Navigator;
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

    
    WorldState  GetCondition            (const WorldState& preState)            const override;
    WorldState  GetEffect               (const WorldState& preState)            const override;
    float       GetCost                 (const WorldState& preState)            const override;
    std::string GetEffectPostfix        (const WorldState& desiredState)        const override;
    float       GetHighestPossibleCost  ()                                      const override;
    WsUserData* ModifyUserData(const WorldState& preState) const override;
};
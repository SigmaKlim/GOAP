﻿#pragma once

#include "../../Attributes/Special/AAtNode.h"
#include "../Basic/IAction.h"
#include "../../Attributes/Special/AEnemyStatus.h"
#include "../../Conditions/Special/CEqual.h"

class AcSearchEnemy : public IAction
{
public:
    AcSearchEnemy(size_t iEnemyStatus, size_t iAtNode, size_t iIsCrouching, float cost) :
    _iEnemyStatus(iEnemyStatus), _iAtNode(iAtNode), _iIsCrouching(iIsCrouching), _cost(cost) {}

    void ConstructActionInstancesPriori(std::vector<ActionInstanceData>& actions, const ConditionSet& requiredConditions, const SupplementalData& userData) override;
    ActionInstanceData ConstructActionInstancePosteriori(const ValueSet& prevState, const ActionInstanceData& prioriActionInstance) override;
    float GetMaxCost() const override;
private:
    size_t _iEnemyStatus;
    size_t _iAtNode;
    size_t _iIsCrouching;
    float _cost;
};

inline void AcSearchEnemy::ConstructActionInstancesPriori(std::vector<ActionInstanceData>& actions, const ConditionSet& requiredConditions,
    const SupplementalData& userData)
{
    ConditionSet cs(numAttributes);
    cs.SetCondition(_iEnemyStatus, new CEqual(EAVEnemyStatus::eNonVisible));
    cs.SetCondition(_iIsCrouching, new CEqual(false));
    ValueSet vs(numAttributes);
    vs.SetValue(_iEnemyStatus, EAVEnemyStatus::eVisible);
    vs.SetValue(_iAtNode, -1);  //we do not know in advance where the agent will appear when the search is over
    float projectedCost = _cost;
    if (userData.futureGoToDestinationNode != -1)
        projectedCost += _cost - AAtNode::navigator.GetDistance(userData.initNode, userData.futureGoToDestinationNode);
    //we update cost according to the information that we will arrive to futureGoToDestinationNode not from
    //initNode but from some unknown node.
    SupplementalData newUserData(userData);
    newUserData.futureGoToDestinationNode = -1;
    actions.push_back({cs, vs, projectedCost, newUserData});
}

inline ActionInstanceData AcSearchEnemy::ConstructActionInstancePosteriori(
    const ValueSet& prevState, const ActionInstanceData& prioriActionInstance)
{
    auto posterioriActionInstance = IAction::ConstructActionInstancePosteriori(prevState, prioriActionInstance);
    int trueDepartureNode = prevState.GetValue(_iAtNode); //now we know it
    posterioriActionInstance.Effects.SetValue(_iAtNode, -1);
    ///implement!!!
    posterioriActionInstance.Cost = _cost;
    return posterioriActionInstance;
}

inline float AcSearchEnemy::GetMaxCost() const
{
    return _cost * 2;
}
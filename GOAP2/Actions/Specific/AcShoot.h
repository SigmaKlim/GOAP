﻿#pragma once
#include "../Basic/IAction.h"
#include "../../Attributes/Special/AEnemyStatus.h"
#include "../../Conditions/Special/CGreater.h"
#include "../../Conditions/Special/CEqual.h"

class AcShoot : public IAction
{
public:
    AcShoot(size_t iAmmoLeftInMag, size_t iEnemyStatus, float cost) : _iAmmoLeftInMag(iAmmoLeftInMag), _iEnemyStatus(iEnemyStatus), _cost(cost){}
    void ConstructActionInstancesPriori(std::vector<ActionInstanceData>& actionInstances, const ConditionSet& requiredConditions, const SupplementalData& userData) override
    {
        ConditionSet conditions(numAttributes);
        conditions.SetCondition(_iAmmoLeftInMag, new CGreater(SHOTS_PER_ACTION - 1));
        conditions.SetCondition(_iEnemyStatus, new CEqual(EAVEnemyStatus::eInRangedCombatRadius));
        ValueSet effects(numAttributes);
        effects.SetValue(_iEnemyStatus, EAVEnemyStatus::eAttacking);
        actionInstances.push_back({conditions, effects, _cost, userData});
    }
    ActionInstanceData ConstructActionInstancePosteriori(const ValueSet& prevState, const ActionInstanceData& prioriActionInstance) override
    {
        auto actionInstance = IAction::ConstructActionInstancePosteriori(prevState, prioriActionInstance);
        auto prevAmmoLeft = prevState.GetProperty(_iAmmoLeftInMag);
        actionInstance.Effects.SetValue(_iAmmoLeftInMag, prevAmmoLeft - SHOTS_PER_ACTION);
        actionInstance.Effects.SetValue(_iEnemyStatus, EAVEnemyStatus::eInRangedCombatRadius);
        actionInstance.StringData = "(ammo left: " + std::to_string(prevAmmoLeft - SHOTS_PER_ACTION) + ")";
        return actionInstance;
    }
    float GetMaxCost() const override
    {
        return _cost;
    }
private:
    size_t _iAmmoLeftInMag;
    size_t _iEnemyStatus;
    float _cost;
    const int SHOTS_PER_ACTION = 5;
    
};
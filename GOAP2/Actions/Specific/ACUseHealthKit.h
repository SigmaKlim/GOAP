#pragma once
#include "../Basic/IActionConstructor.h"
#include "../../Condition/Special/CLarger.h"

class ACUseHealthKit : public IActionConstructor
{
public:
    ACUseHealthKit(size_t numDepletablesAttributeIndex, size_t affectedAttributeIndex, t_value delta) :
    _numHKitsIndex(numDepletablesAttributeIndex), _hpLeftIndex(affectedAttributeIndex),
    _delta(delta) {}
    void ConstructActions(std::vector<Action>& actions, const ConditionSet& requiredConditions, const ActionData& userData) override;
    float GetMaxCost() const override;
    
private:
    size_t _numHKitsIndex;   //index of the attribute showing the minimal required number of depletables
    size_t _hpLeftIndex;     //index of the attribute which value is affected by using the depletable (medkit, ammo)
    t_value _delta;          //hpLeft += delta
};

inline void ACUseHealthKit::ConstructActions(std::vector<Action>& actions, const ConditionSet& requiredConditions,
    const ActionData& userData)
{
    int minNumDepletables = 1;
    if (requiredConditions.IsAffected(_numHKitsIndex) == true)
    {
        auto* cLargerNumDepletables = dynamic_cast<const CLarger*>(requiredConditions.GetProperty(_numHKitsIndex).get());
        assert(cLargerNumDepletables);
        minNumDepletables = cLargerNumDepletables->Value + 1; //Make minimal required number of depletables larger by 1, as this action depletes 1 item
    }
    ConditionSet cs(numAttributes);
    cs.SetCondition(_numHKitsIndex, new CLarger(minNumDepletables));
    auto* cLargerAffected = dynamic_cast<const CLarger*>(requiredConditions.GetProperty(_hpLeftIndex).get());
    assert(cLargerAffected); //!!!
    t_value requiredValue = cLargerAffected->Value; 
    cs.SetCondition(_hpLeftIndex, new CLarger(requiredValue - _delta)); //Make minimal required value of the affected attribute less by delta, as this action increases it by delta
    ValueSet vs(numAttributes);
    vs.SetValue(_hpLeftIndex, requiredValue);
    Action action;
}

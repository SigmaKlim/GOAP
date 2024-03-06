#pragma once
#include <unordered_set>

#include "../Basic/ICondition.h"
//assume that we use InSet only for enum-like attributes, so the distance between two values is 1 or 0
struct CInSet : public ICondition
{
    CInSet(const std::unordered_set<t_value>& valueSet, IAttribute* attributePtr = nullptr);
    float Evaluate(t_value value) const override;

    //Condition resolution

    RESOLVE_D(CInSet)
    ICondition* ResolveCEqual(const CEqual* cEqual) const override;
    ICondition* ResolveCLarger(const CLarger* cLargerOrEqual) const override;
    ICondition* ResolveCInSet(const CInSet* cInSet) const override;
    
    std::unordered_set<t_value> Values;
};



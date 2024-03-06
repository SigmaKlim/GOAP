#pragma once
#include "../Basic/ICondition.h"

struct CEqual : public ICondition
{
    CEqual(t_value value, IAttribute* attributePtr = nullptr);
    float Evaluate(t_value value) const override;

    //Condition resolution
    
    RESOLVE_D(CEqual)
    ICondition* ResolveCEqual(const CEqual* cEqual) const override;
    ICondition* ResolveCLarger(const CLarger* cLarger) const override;
    ICondition* ResolveCInSet(const CInSet* cInSet) const override;

    t_value Value;
};


#pragma once

#include "ICondition.h"
struct Equal : public ICondition
{
    Equal(t_value value) : Value(value) {}
    float Evaluate(t_value value, const IAttribute* iAttributePtr) const override;

    //Condition resolution
    
    RESOLVE_D(Equal)
    ICondition* ResolveEqual(const Equal* p_Equal) const override;
    ICondition* ResolveLargerOrEqual(const LargerOrEqual* p_LargerOrEqual) const override;
    
    t_value Value;
};

struct LargerOrEqual :  public ICondition
{
    LargerOrEqual(t_value value) : Value(value) {}
    float Evaluate(t_value value, const IAttribute* iAttributePtr) const override;

    //Condition resolution
    
    RESOLVE_D(LargerOrEqual)
    
    ICondition* ResolveLargerOrEqual(const LargerOrEqual* p_LargerOrEqual) const override;
    ICondition* ResolveEqual(const Equal* p_Equal) const override;

    t_value Value;
};
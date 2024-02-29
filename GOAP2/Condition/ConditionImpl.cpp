#include "ConditionImpl.h"

float Equal::Evaluate(t_value value, const IAttribute* iAttributePtr) const
{
    return iAttributePtr->GetDifference(value, Value);
}

ICondition* Equal::ResolveEqual(const Equal* p_Equal) const
{
    return Value == p_Equal->Value ? new Equal(Value) : nullptr;
}

ICondition* Equal::ResolveLargerOrEqual(const LargerOrEqual* p_LargerOrEqual) const
{
    return p_LargerOrEqual->ResolveEqual(this);
}

float LargerOrEqual::Evaluate(t_value value, const IAttribute* iAttributePtr) const
{
    return value >= Value ? 0.0f : iAttributePtr->GetDifference(value, Value);
}

ICondition* LargerOrEqual::ResolveLargerOrEqual(const LargerOrEqual* p_LargerOrEqual) const
{
    const t_value& a = Value;
    const t_value& b = p_LargerOrEqual->Value;
    if (a >= b)
        return new LargerOrEqual(a);
    return new LargerOrEqual(b);
}

ICondition* LargerOrEqual::ResolveEqual(const Equal* p_Equal) const
{
    if (p_Equal->Value >= Value)
        return new Equal(Value);
    return nullptr;
}

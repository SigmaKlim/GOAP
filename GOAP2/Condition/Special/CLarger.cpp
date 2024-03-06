#include "CEqual.h"
#include "CInSet.h"
#include "CLarger.h"

CLarger::CLarger(t_value value, IAttribute* attributePtr): Value(value), ICondition(attributePtr)
{}

float CLarger::Evaluate(t_value value) const
{
    return value > Value ? 0.0f : _attributePtr->GetDifference(value, Value);
}

ICondition* CLarger::ResolveCLarger(const CLarger* cLarger) const
{
    const t_value& a = Value;
    const t_value& b = cLarger->Value;
    if (a > b)
        return new CLarger(a, _attributePtr);
    return new CLarger(b, _attributePtr);
}

ICondition* CLarger::ResolveCEqual(const CEqual* cEqual) const
{
    if (cEqual->Value > Value)
        return new CEqual(Value, _attributePtr);
    return nullptr;
}

ICondition* CLarger::ResolveCInSet(const CInSet* cInSet) const
{
    return cInSet->ResolveCLarger(this);
}
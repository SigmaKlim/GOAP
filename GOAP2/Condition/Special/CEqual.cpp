#include "CEqual.h"
#include "CInSet.h"
#include "CLarger.h"
CEqual::CEqual(t_value value, IAttribute* attributePtr) : Value(value), ICondition(attributePtr)
{}

float CEqual::Evaluate(t_value value) const
{
    return _attributePtr->GetDifference(value, Value);
}

ICondition* CEqual::ResolveCEqual(const CEqual* cEqual) const
{
    return Value == cEqual->Value ? new CEqual(Value, _attributePtr) : nullptr;
}

ICondition* CEqual::ResolveCLarger(const CLarger* cLarger) const
{
    return cLarger->ResolveCEqual(this);
}

ICondition* CEqual::ResolveCInSet(const CInSet* cInSet) const
{
    return cInSet->ResolveCEqual(this);
}

#include "CEqual.h"
#include "CInSet.h"
#include "CLarger.h"

CInSet::CInSet(const std::unordered_set<t_value>& valueSet, IAttribute* attributePtr): Values(valueSet), ICondition(attributePtr)
{}

float CInSet::Evaluate(t_value value) const
{
    return (float)(!Values.contains(value)); //we assume that we deal with enum-like attribute
}

ICondition* CInSet::ResolveCEqual(const CEqual* cEqual) const
{
    return Values.contains(cEqual->Value) ? new CEqual(cEqual->Value, _attributePtr) : nullptr;
}

ICondition* CInSet::ResolveCLarger(const CLarger* cLarger) const
{
    std::unordered_set<t_value> goodValues;
    for (auto& value : Values)
        if (value < cLarger->Value)
            goodValues.insert(value);
    return goodValues.empty() ? nullptr : new CInSet(goodValues, _attributePtr);
}


ICondition* CInSet::ResolveCInSet(const CInSet* cInSet) const
{
    std::unordered_set<t_value> goodValues;
    for (auto& value : Values)
        if (cInSet->Values.contains(value))
            goodValues.insert(value);
    return goodValues.empty() ? nullptr : new CInSet(goodValues, _attributePtr);
}
#include "Helper.h"

Helper::Helper(Planner* plannerPtr) : _plannerPtr(plannerPtr) {}

ConditionSet Helper::MakeConditionSet(const std::map<std::string, ICondition*>& input)
{
    ConditionSet cs(_plannerPtr->GetNumAttributes());
    for (auto& pair : input)
    {
        auto* uncookedConditionPtr = pair.second; //the uncooked condition pointer with attribute not set
        auto* cookedConditionPtr = uncookedConditionPtr;
        cookedConditionPtr->_attributePtr = *_plannerPtr->_attributeCatalogue.GetItem(pair.first); //set attribute ptr here
        cs.SetCondition(_plannerPtr->GetAttributeId(pair.first), cookedConditionPtr);
    }
    return cs;
}

ValueSet Helper::MakeValueSet(const std::map<std::string, t_value>& input)
{
    ValueSet vs(_plannerPtr->GetNumAttributes());
    for (auto& pair : input)
        vs.SetValue(_plannerPtr->GetAttributeId(pair.first), pair.second);
    return vs;
}

#pragma once

#include "Planner.h"

class Helper
{
public:
    Helper(Planner* plannerPtr);
    //input: attributeName->ICondition*
    ConditionSet MakeConditionSet(const std::map<std::string, ICondition*>& input);
    //input: attributeName->attributeValue
    ValueSet MakeValueSet(const std::map<std::string, t_value>& input);

    //Cast from t_super* to t_sub* and assert that result is not nullptr
    template <typename t_sub, typename t_super>
    static t_sub* CastAssert(t_super* supPtr)
    {
        auto* subPtr = dynamic_cast<t_sub*>(supPtr);
        assert(subPtr);
        return subPtr;
    }
private:
    Planner* _plannerPtr;
};
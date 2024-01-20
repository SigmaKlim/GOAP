#pragma once
#include <string>
#include <unordered_map>



//attribute as a simple unordered set of values
class SimpleAttribute : public Attribute
{
public:
    SimpleAttribute(const std::vector<std::string>& enumeratorNames) : Attribute(enumeratorNames) {};
    
    float GetDifference(unsigned attributeConditionMask, unsigned attributeTargetMask) const override
    {
        return attributeConditionMask != 0 &&                                       //simple check if there is no masked condition
            (attributeConditionMask & attributeTargetMask) != attributeTargetMask;  //or it is similar to target value, return 0 if so, otherwise - 1
    }
    float GetMaxDifference() const override
    {
        return 1.0f;
    }
};
#pragma once
#include <string>
#include <unordered_map>

#include "Attribute.h"

//attribute as a simple unordered set of values
class SimpleAttribute : public Attribute
{
public:
    SimpleAttribute(const std::vector<std::string>& enumeratorNames) : Attribute(enumeratorNames) {};
    unsigned GetDifference(unsigned attributeCondition, unsigned attributeTarget) const override
    {
        return (attributeCondition & attributeTarget) == attributeTarget; //simple check if masked condition is similar to target value, return 1 if so, otherwise - 0
    }

};